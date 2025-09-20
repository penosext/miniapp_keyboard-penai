rem Copyright (C) 2025 Langning Chen
rem 
rem This file is part of miniapp.
rem 
rem miniapp is free software: you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation, either version 3 of the License, or
rem (at your option) any later version.
rem 
rem miniapp is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem 
rem You should have received a copy of the GNU General Public License
rem along with miniapp.  If not, see <https://www.gnu.org/licenses/>.

adb -s 7E92000008705369 push Z:\home\langningchen\miniapp\dist\miniapp-arm-buildroot-linux-gnueabihf.amr /userdisk/Favorite/miniapp.amr & adb -s 7E92000008705369 shell miniapp_cli install /userdisk/Favorite/miniapp.amr & adb -s 7E92000008705369 shell miniapp_cli start 8001749644971193
adb -s 7E92000008705369 shell cat /userdata/applog/DictPen_*.log
adb -s 7E92000008705369 pull /userdisk/database/langningchen-ai.db langningchen-ai.db
adb -s 7E92000008705369 push langningchen-ai.db /userdisk/database/langningchen-ai.db
adb -s 7E92000008705369 shell rm /userdisk/database/langningchen-ai.db
