//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

//
// TODO generated message class
//
cplusplus {{
typedef std::pair<int, unsigned int> Header;
// first is MessageID, second is its timestamp
}}

class noncobject Header;

packet Frame {
    @customize(true);  // see the generated C++ header for more info
    Header header;
    string payload;
    char trailer;
    bool ack;
    int piggyback_id;
//    piggyback_id is ID of next expected frame
}
