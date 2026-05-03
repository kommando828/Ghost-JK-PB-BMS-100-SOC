# Ghost-JK-PB-BMS-100-SOC
Ghost JK PB BMS 100% SOC

This is a rewite of the Ghost JK PB BMS Fizer which was developed to stop the master JK PB BMS resporting 100% SOC prematurely. It emulated a small 0% SOC battery that took any bank of BMS's average down from 100% to 99%

This version is the opposite. It emulates a 304AH battery at 100% to keep the SOC average up, so 4 280AH to 320AH BMS's at 0% plus this Ghost at 100% will average approx 20%.

It runs on an M%Stack AtomS3 Lite with the RS485 addon and connects to the last Urat2 RJ45 socket on the last real Battery.

There are editable fields and as long as you restrict edits to these fields the program will recalc the checksums the JK PB BMS master expects to see.

If you ever want to tune it further, the only constants you need to touch are at the top of JKModbusSlave.cpp:
constexpr uint32_t SPOOF_CAPACITY_MAH = 304000; // increase to give ghost more weight
constexpr uint32_t SPOOF_VOLTAGE_MV   =  53000;
constexpr uint8_t  SPOOF_SOC_PERCENT  =    100;
constexpr uint8_t  SPOOF_SOH_PERCENT  =    100;
constexpr uint32_t SPOOF_CYCLE_COUNT  =     10;
// And in _frame2(): writeUint32LE(_buf, 194, 88634); for runtime where 88634 is in secs
