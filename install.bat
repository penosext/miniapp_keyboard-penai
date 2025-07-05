adb push D:\8001749644971193.0_0_1.amr /userdisk/Favorite/miniapp.amr & adb shell miniapp_cli install /userdisk/Favorite/miniapp.amr & adb shell miniapp_cli start 8001749644971193
adb shell "cat /userdata/applog/DictPen_*.log | tail -n 50"
adb shell "rm /userdata/applog/DictPen_*.log"
