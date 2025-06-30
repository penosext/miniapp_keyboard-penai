// Copyright (C) 2025 Langning Chen
// 
// This file is part of miniapp.
// 
// miniapp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// miniapp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with miniapp.  If not, see <https://www.gnu.org/licenses/>.

import { defineComponent } from 'vue';
import { SoftKeyboardEvent } from './softKeyboard';

const component = defineComponent({
    data() {
        return {
            inputResult: ['test'],
        };
    },
    methods: {
        loadSoftKeyboard() {
            $falcon.navTo('softKeyboard', { data: this.inputResult.join('\n') });
            $falcon.on<SoftKeyboardEvent>('softKeyboard', (e) => {
                this.inputResult = e.data.data.split('\n');
            });
        },
    }
});

export default component;
