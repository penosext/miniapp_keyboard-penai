<!--
 Copyright (C) 2025 Langning Chen
 
 This file is part of miniapp.
 
 miniapp is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 miniapp is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with miniapp.  If not, see <https://www.gnu.org/licenses/>.
-->

# Miniapp

> [!WARNING]  
> This miniapp is still under development, and may have breaking changes without any notice.

[![Build](https://github.com/langningchen/miniapp/actions/workflows/build.yml/badge.svg)](https://github.com/langningchen/miniapp/actions/workflows/build.yml)

## Requirements

Users using this miniapp should have a [supported YouDao Dictionary Pen](https://smart.youdao.com/dictPen).

## Installation

1. Make sure you have a YouDao Dictionary Pen with `adb` enabled. You can refer to [these discussions](https://github.com/orgs/PenUniverse/discussions/).
2. Go to the latest [GitHub Actions](https://github.com/langningchen/miniapp/actions/workflows/build.yml) and download the `miniapp.amr` file from section `Artifacts`.
3. Connect your YouDao Dictionary Pen to your computer and login to it using `adb shell auth`.
4. Upload the `miniapp.amr` file to your YouDao Dictionary Pen using `adb push`:
   ```bash
   adb push miniapp.amr /userdisk/Favorite/miniapp.amr
   ```
5. Open the shell using `adb shell` and run the following command to install the miniapp:
   ```bash
   miniapp_cli install /userdisk/Favorite/miniapp.amr
   ```
6. After installation, you can find the miniapp in the app list of your YouDao Dictionary Pen.

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

## Known Issues

See [GitHub Issues](https://github.com/langningchen/miniapp/issues).
