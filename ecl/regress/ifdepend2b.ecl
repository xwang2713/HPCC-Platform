/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems®.

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

#option ('targetClusterType', 'hthor');


namesRecord :=
            RECORD
string20        surname;
string10        forename;
integer2        age := 25;
            END;

namesTable := dataset([
        {'Hawthorn','Gavin',31},
        {'Hawthorn','Mia',30},
        {'Zingo','Pru',10},
        {'X','Z'}], namesRecord);


s := sort(namesTable, surname);
makeSplit1 := s(surname != '');

boolean isOk := true : stored('isOkay');

x1 := if(isOk, s, makeSplit1);
x2 := if(isOk, nofold(x1), makeSplit1);

output(x2);
