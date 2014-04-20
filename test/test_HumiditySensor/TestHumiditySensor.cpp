#include <QtCore>
#include <QtTest>

#include "HumiditySensor.h"

class TestHumiditySensor : public QObject
{
    Q_OBJECT

    private:
    public:

    private slots:
        // Tests for the HumiditySensor class
        void test_valueTimeToSend();
        void test_alarm();
        void test_offset();
        void test_offset_data();
};


/**
 * Test that we do not send to many duplicate data.
 */
void TestHumiditySensor::test_valueTimeToSend()
{
    HumiditySensor sensor;

    bool res = sensor.valueTimeToSend(10.0, 60.0);
    QCOMPARE(res, true); //Always send the first time...
    for(int i=1; i<=ALWAYS_SEND_CNT; i++)
    {
        res = sensor.valueTimeToSend(10.0, 60.0);
        if(true == res)
        {
            qDebug() << "Error we should not send now" << i;
            QFAIL("Error valueSendCnt wrong");
        }
    }

    sensor.setDiffToSend(0.5, 5);

    //Then we shall send
    QCOMPARE(sensor.valueTimeToSend(20.0, 60.0), true);
    //And we are back in the loop
    QCOMPARE(sensor.valueTimeToSend(20.0, 60.0), false);

    //Now check with a +diff
    QCOMPARE(sensor.valueTimeToSend(21.0, 60.0), true);  //21-20  = 1  > 0.5 => OK to send
    QCOMPARE(sensor.valueTimeToSend(20.8, 60.0), false); //21-20.8= 0.2< 0.5 => Not ok to send
    QCOMPARE(sensor.valueTimeToSend(20.8, 70.0), true);  //70-60  = 10 > 5   => OK
    QCOMPARE(sensor.valueTimeToSend(20.8, 68.0), false); //70-68  =  2 < 5   => Not ok to send

    //Now check with a -diff
    QCOMPARE(sensor.valueTimeToSend(20.8, 60.0), true); //68-60 = 8 > 5 => ok to send
    QCOMPARE(sensor.valueTimeToSend(20.0, 60.0), true); //20.8-20  =  0.8 > 0.2 => ok to send
    QCOMPARE(sensor.valueTimeToSend(20.0, 60.0), false);
}


/**
 * Test the alarm functions
 * setAlarmLevels, alarmHighCheck, alarmLowCheck, alarmHighFailed, alarmLowFailed
 */
void TestHumiditySensor::test_alarm()
{
    HumiditySensor sensor;

    //No alarms, never return true
    sensor.setAlarmLevels(
            30, false,
            20, false,
            80, false,
            30, false);

#define MAX_STR 80
    //Get a little bit more space,
    //and fill that with 0xFF so we can check that we did not touch it!
    char str[MAX_STR+4];
    memset(&str, 0xFF, MAX_STR+4);

    /*
    for(char i=0; i<MAX_STR+4; i++)
    {
        printf(" %02d - 0x%x\n", i, str[i]);
    }
    */

    //Since all is off, no alarms!
    for(double val=10; val<90; val+=10)
    {
        sensor.valueTimeToSend(val, val);
        QCOMPARE(sensor.alarmCheck(str, MAX_STR), false);
    }

    // ---------------------------
    // Part1 Temperature High
    // ---------------------------
    sensor.setAlarmLevels(
            30, true,
            20, false,
            80, false,
            30, false);
    sensor.setAlarmHyst(2.0, 5.0);

    sensor.valueTimeToSend(15.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false);
    sensor.valueTimeToSend(25.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false);
    sensor.valueTimeToSend(35.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    qDebug() << str;
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Only send once!
    //But if we are told that it failed, we get another try.
    sensor.alarmFailed();
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    //qDebug() << str;


    //Check the Hysteresis
    sensor.valueTimeToSend(31.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Only send once
    sensor.valueTimeToSend(29.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Under level, but not under level+hyst
    sensor.valueTimeToSend(31.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Still no go since it has not been reset
    sensor.valueTimeToSend(27.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Under level+hyst, so we reset the alarm (but it is not active)
    sensor.valueTimeToSend(31.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true); // And now we get a alarm again.



    // ---------------------------
    // Part2 Temperature Low
    // ---------------------------
    sensor.setAlarmLevels(
            30, false,
            20, true,
            80, false,
            30, false);
    sensor.setAlarmHyst(2.0, 5.0);

    sensor.valueTimeToSend(25.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false);
    sensor.valueTimeToSend(15.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    qDebug() << str;
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Only send once!
    //But if we are told that it failed, we get another try.
    sensor.alarmFailed();
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    //qDebug() << str;


    //Check the Hysteresis
    sensor.valueTimeToSend(15.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Only send once
    sensor.valueTimeToSend(21.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Over level, but not over level+hyst
    sensor.valueTimeToSend(19.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Still no go since it has not been reset
    sensor.valueTimeToSend(25.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Over level+hyst, so we reset the alarm (but it is not active)
    sensor.valueTimeToSend(19.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true); // And now we get a alarm again.



    // ---------------------------
    // Part3 Humidity High
    // ---------------------------
    sensor.setAlarmLevels(
            30, false,
            20, false,
            60, true,
            30, false);
    sensor.setAlarmHyst(2.0, 4.0);

    sensor.valueTimeToSend(15.0, 50.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false);
    sensor.valueTimeToSend(25.0, 55.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false);
    sensor.valueTimeToSend(35.0, 65.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    qDebug() << str;
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Only send once!
    //But if we are told that it failed, we get another try.
    sensor.alarmFailed();
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    //qDebug() << str;


    //Check the Hysteresis
    sensor.valueTimeToSend(31.0, 61.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Only send once
    sensor.valueTimeToSend(29.0, 59.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Under level, but not under level+hyst
    sensor.valueTimeToSend(31.0, 61.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Still no go since it has not been reset
    sensor.valueTimeToSend(27.0, 55.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Under level+hyst, so we reset the alarm (but it is not active)
    sensor.valueTimeToSend(31.0, 61.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true); // And now we get a alarm again.


    // ---------------------------
    // Part4 Humidity Low
    // ---------------------------
    sensor.setAlarmLevels(
            30, false,
            20, false,
            80, false,
            30, true);
    sensor.setAlarmHyst(2.0, 4.0);

    sensor.valueTimeToSend(25.0, 35.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false);
    sensor.valueTimeToSend(15.0, 25.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    qDebug() << str;
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Only send once!
    //But if we are told that it failed, we get another try.
    sensor.alarmFailed();
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    //qDebug() << str;


    //Check the Hysteresis
    sensor.valueTimeToSend(15.0, 25.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Only send once
    sensor.valueTimeToSend(21.0, 31.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Over level, but not over level+hyst
    sensor.valueTimeToSend(19.0, 29.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Still no go since it has not been reset
    sensor.valueTimeToSend(25.0, 35.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false); //Over level+hyst, so we reset the alarm (but it is not active)
    sensor.valueTimeToSend(19.0, 29.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true); // And now we get a alarm again.




    // ---------------------------
    // Part Everything ON
    // ---------------------------

    //Activate all alarms!
    sensor.setAlarmLevels(
            30, true,
            20, true,
            80, true,
            40, true);

    sensor.valueTimeToSend(15.0, 35.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    qDebug() << str;

    sensor.valueTimeToSend(40.0, 90.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), true);
    qDebug() << str;

    sensor.valueTimeToSend(25.0, 60.0);
    QCOMPARE(sensor.alarmCheck(str, MAX_STR), false);


    // --------------------------------------------------
    // Check that the reserved space is still untouched!
    // --------------------------------------------------
    /*
    for(char i=MAX_STR; i<MAX_STR+4; i++)
    {
        //printf(" %02d - 0x%x\n", i, str[i]);
        QCOMPARE((char)str[i], (char)0xFF);
    }
    */
}


void TestHumiditySensor::test_offset_data()
{
    QTest::addColumn<double>("temperatureValue");
    QTest::addColumn<double>("temperatureOffset");
    QTest::addColumn<double>("temperatureResult");

    QTest::addColumn<double>("humidityValue");
    QTest::addColumn<double>("humidityOffset");
    QTest::addColumn<double>("humidityResult");

    QTest::newRow("No offset")
        << 15.0 <<  0.0 << 15.0
        << 40.0 <<  0.0 << 40.0;
    QTest::newRow("Offset +1")
        << 10.0 <<  1.0 << 11.0
        << 45.0 <<  2.0 << 47.0;
    QTest::newRow("Offset -1")
        << 10.0 << -1.0 <<  9.0
        << 25.0 << -2.0 << 23.0;
    QTest::newRow("Offset +9.5")
        << 10.0 <<  9.5 << 19.5
        << 20.5 <<  9.5 << 30.0;
    QTest::newRow("Offset -9.5")
        << 10.0 << -9.5 <<  0.5
        << 19.5 << -9.5 << 10.0;
}

/**
 * Check that the offset is correct
 */
void TestHumiditySensor::test_offset()
{
    QFETCH(double, temperatureValue);
    QFETCH(double, temperatureOffset);
    QFETCH(double, temperatureResult);

    QFETCH(double, humidityValue);
    QFETCH(double, humidityOffset);
    QFETCH(double, humidityResult);

    HumiditySensor sensor;

    sensor.setValueOffset(temperatureOffset, humidityOffset);
    sensor.valueTimeToSend(temperatureValue, humidityValue);

    QCOMPARE(sensor.temperatureWork, temperatureResult);
    QCOMPARE(sensor.humidityWork, humidityResult);
}


QTEST_MAIN(TestHumiditySensor)
#include "TestHumiditySensor.moc"
