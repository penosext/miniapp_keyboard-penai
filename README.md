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

## Requirements

Users using this miniapp should have a [supported YouDao Dictionary Pen](https://smart.youdao.com/dictPen).

## Preparation

1. Make sure you have a YouDao Dictionary Pen with `adb` enabled. You can use [the paper tool](https://github.com/langningchen/paper) to edit adb password easily or refer to [these discussions](https://github.com/orgs/PenUniverse/discussions/).
2. Connect your YouDao Dictionary Pen to your computer and login to it using `adb shell auth`.
3. Make sure you have `cmake`, `make`, `nodejs`, `pnpm`, `iconv` installed on a Ubuntu computer.
4. Clone this repository:
   ```bash
   git clone https://github.com/langningchen/miniapp.git
   cd miniapp
   ```

## Build

On Android:

1. Open the shell using `adb shell` and run the following command to check the version of system:
   ```bash
   curl -k -s https://raw.githubusercontent.com/langningchen/miniapp/refs/heads/main/tools/getVersionInfo.sh | bash
   ```
2. Pull the artifacts from the YouDao Dictionary Pen:
   ```bash
   adb pull /userdisk/Favorite/versionInfo.tar.gz ./versionInfo.tar.gz
   ```

On Ubuntu:

1. Extract the `versionInfo.tar.gz` file:
   ```bash
   tar -xzf versionInfo.tar.gz -C ./jsapi
   ```
2. Download and extract the correct toolchain in the `jsapi/toolchains` directory,
   your directory structure should look like this:
   ```
   miniapp/
   ├── jsapi/
   |   |-- include/
   |   |   |-- curl/
   |   |   |-- sqlite3/
   |   |-- lib/
   |   |   ├── libcurl.so
   |   |   ├── libsqlite3.so
   │   ├── src/
   │   ├── toolchains/
   │   │   ├── <toolchain_name>/
   │   │   │   ├── bin/
   │   │   │   ├── include/
   │   │   │   └── lib/
   ```
3. Install NodeJS dependencies using `pnpm`:
   ```bash
   pnpm -C ui install
   ```
4. Modify the NodeJS library: 
   ```bash
   cd ./ui
   sed -i "s/commonjs(),/commonjs(),require('@rollup\/plugin-typescript')(),/g" ./node_modules/aiot-vue-cli/src/libs/rollup.config.js
   sed -i "s/compiler.parseComponent(content, { pad: 'line' })/compiler.parse(content, { pad: 'line' }).descriptor/g" ./node_modules/aiot-vue-cli/web-loaders/falcon-vue-loader/lib/parser.js
   sed -i "s/path.resolve(__dirname, '.\/vue\/packages\/vue-template-compiler\/index.js')/'@vue\/compiler-sfc'/g" ./node_modules/aiot-vue-cli/cli-libs/index.js
   sed -i "s/compiler.parseComponent(content, { pad: true })/compiler.parse(content, { pad: true }).descriptor/g" ./node_modules/aiot-vue-cli/src/libs/parser.js
   sed -i "s/compiler.compile/compiler.compileTemplate/g" ./node_modules/aiot-vue-cli/web-loaders/falcon-vue-loader/lib/template-compiler/index.js
   sed -i "s/const replaceValues = {}/const replaceValues = { 'defineComponent': '' }/g" ./node_modules/aiot-vue-cli/src/libs/rollup.config.js
   ```
5. Run the build script:
   ```bash
   ./tools/build.sh
   ```
6. After the build is complete, you will find the `miniapp.amr` file in the `dist` directory.

## Installation

1. Upload the `miniapp.amr` file to your YouDao Dictionary Pen using `adb push`:
   ```bash
   adb push miniapp.amr /userdisk/Favorite/miniapp.amr
   ```
2. Open the shell using `adb shell` and run the following command to install the miniapp:
   ```bash
   miniapp_cli install /userdisk/Favorite/miniapp.amr
   ```
3. After installation, you can find the miniapp in the app list of your YouDao Dictionary Pen.

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

## Known Issues

See [GitHub Issues](https://github.com/langningchen/miniapp/issues).
