# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

def addvars2notebook(filename, vars_and_values):
    stream = open(filename)
    lines = stream.readlines()
    stream.close()
    newlines = []
    for line in lines:
        if line.find("= geompy.") >= 0:
            name = line.split('=')[0]
            name = name.strip()
            vals = line
            fields = vals.split("(")
            if len(fields) == 2:
                begin = fields[0] + "("
                vals = fields[1]
                fields = vals.split(")")
                if len(fields) == 2:
                    vals = fields[0]
                    end = ")" + fields[1]
                    vals = vals.split(',')
                    newline  = begin
                    newvals = []
                    for i in range(len(vals)):
                        valname = name + "_val_%s"%(i+1)
                        val = vals[i]
                        vvv = val.strip()
                        try:
                            iii = int(vvv)
                            vars_and_values.append([valname, val])
                            val = val.replace(vvv, valname.__repr__())
                        except ValueError:
                            try:
                                fff = float(vvv)
                                vars_and_values.append([valname, val])
                                val = val.replace(vvv, valname.__repr__())
                            except ValueError:
                                pass
                            pass
                        newvals.append(val)
                        pass
                    newline += ','.join(newvals)
                    newline += end
                    line = newline
                    pass
                pass
            pass
        newlines.append(line)
        pass
    content = "".join(newlines)
    return content
