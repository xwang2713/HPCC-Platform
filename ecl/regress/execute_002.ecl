/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2025 HPCC Systems®.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

IMPORT Std;

// HPCC-32793

// Assumes execution on mythor1 thor cluster

ds1 := NOFOLD(DATASET(10000, TRANSFORM({UNSIGNED4 n}, SELF.n := RANDOM()), DISTRIBUTED));

// Data: Build data on a different thor cluster
EXECUTE('mythor2', ds1);
