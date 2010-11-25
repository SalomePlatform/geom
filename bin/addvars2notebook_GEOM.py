
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
