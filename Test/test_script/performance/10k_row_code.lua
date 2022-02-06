#!../lua
-- $Id: testes/all.lua $
-- See Copyright Notice at the end of this file


local version = "Lua 5.4"
if _VERSION ~= version then
  io.stderr:write("This test suite is for ", version,
                  ", not for ", _VERSION, "\nExiting tests")
  return
end


_G.ARG = arg   -- save arg for other tests


-- next variables control the execution of some tests
-- true means no test (so an undefined variable does not skip a test)
-- defaults are for Linux; test everything.
-- Make true to avoid long or memory consuming tests
_soft = rawget(_G, "_soft") or false
-- Make true to avoid non-portable tests
_port = rawget(_G, "_port") or false
-- Make true to avoid messages about tests not performed
_nomsg = rawget(_G, "_nomsg") or false


local usertests = rawget(_G, "_U")

if usertests then
  -- tests for sissies ;)  Avoid problems
  _soft = true
  _port = true
  _nomsg = true
end

-- tests should require debug when needed
debug = nil


if usertests then
  T = nil    -- no "internal" tests for user tests
else
  T = rawget(_G, "T")  -- avoid problems with 'strict' module
end


--[=[
  example of a long [comment],
  [[spanning several [lines]]]

]=]

print("\n\tStarting Tests")

do
  -- set random seed
  local random_x, random_y = math.randomseed()
  print(string.format("random seeds: %d, %d", random_x, random_y))
end

print("current path:\n****" .. package.path .. "****\n")


local initclock = os.clock()
local lastclock = initclock
local walltime = os.time()

local collectgarbage = collectgarbage

do   -- (

-- track messages for tests not performed
local msgs = {}
function Message (m)
  if not _nomsg then
    print(m)
    msgs[#msgs+1] = string.sub(m, 3, -3)
  end
end

assert(os.setlocale"C")

local T,print,format,write,assert,type,unpack,floor =
      T,print,string.format,io.write,assert,type,table.unpack,math.floor

-- use K for 1000 and M for 1000000 (not 2^10 -- 2^20)
local function F (m)
  local function round (m)
    m = m + 0.04999
    return format("%.1f", m)      -- keep one decimal digit
  end
  if m < 1000 then return m
  else
    m = m / 1000
    if m < 1000 then return round(m).."K"
    else
      return round(m/1000).."M"
    end
  end
end

local Cstacklevel

local showmem
if not T then
  local max = 0
  showmem = function ()
    local m = collectgarbage("count") * 1024
    max = (m > max) and m or max
    print(format("    ---- total memory: %s, max memory: %s ----\n",
          F(m), F(max)))
  end
  Cstacklevel = function () return 0 end   -- no info about stack level
else
  showmem = function ()
    T.checkmemory()
    local total, numblocks, maxmem = T.totalmem()
    local count = collectgarbage("count")
    print(format(
      "\n    ---- total memory: %s (%.0fK), max use: %s,  blocks: %d\n",
      F(total), count, F(maxmem), numblocks))
    print(format("\t(strings:  %d, tables: %d, functions: %d, "..
                 "\n\tudata: %d, threads: %d)",
                 T.totalmem"string", T.totalmem"table", T.totalmem"function",
                 T.totalmem"userdata", T.totalmem"thread"))
  end

  Cstacklevel = function ()
    local _, _, ncalls = T.stacklevel()
    return ncalls    -- number of C calls
  end
end


local Cstack = Cstacklevel()

--
-- redefine dofile to run files through dump/undump
--
local function report (n) print("\n***** FILE '"..n.."'*****") end
local olddofile = dofile
local dofile = function (n, strip)
  showmem()
  local c = os.clock()
  print(string.format("time: %g (+%g)", c - initclock, c - lastclock))
  lastclock = c
  report(n)
  local f = assert(loadfile(n))
  local b = string.dump(f, strip)
  f = assert(load(b))
  return f()
end

dofile('main.lua')

-- trace GC cycles
require"tracegc".start()

report"gc.lua"
local f = assert(loadfile('gc.lua'))
f()

dofile('db.lua')
assert(dofile('calls.lua') == deep and deep)
olddofile('strings.lua')
olddofile('literals.lua')
dofile('tpack.lua')
assert(dofile('attrib.lua') == 27)
dofile('gengc.lua')
assert(dofile('locals.lua') == 5)
dofile('constructs.lua')
dofile('code.lua', true)
if not _G._soft then
  report('big.lua')
  local f = coroutine.wrap(assert(loadfile('big.lua')))
  assert(f() == 'b')
  assert(f() == 'a')
end
dofile('cstack.lua')
dofile('nextvar.lua')
dofile('pm.lua')
dofile('utf8.lua')
dofile('api.lua')
assert(dofile('events.lua') == 12)
dofile('vararg.lua')
dofile('closure.lua')
dofile('coroutine.lua')
dofile('goto.lua', true)
dofile('errors.lua')
dofile('math.lua')
dofile('sort.lua', true)
dofile('bitwise.lua')
assert(dofile('verybig.lua', true) == 10); collectgarbage()
dofile('files.lua')

if #msgs > 0 then
  local m = table.concat(msgs, "\n  ")
  warn("#tests not performed:\n  ", m, "\n")
end

print("(there should be two warnings now)")
warn("@on")
warn("#This is ", "an expected", " warning")
warn("@off")
warn("******** THIS WARNING SHOULD NOT APPEAR **********")
warn("******** THIS WARNING ALSO SHOULD NOT APPEAR **********")
warn("@on")
warn("#This is", " another one")

-- no test module should define 'debug'
assert(debug == nil)

local debug = require "debug"

print(string.format("%d-bit integers, %d-bit floats",
        string.packsize("j") * 8, string.packsize("n") * 8))

debug.sethook(function (a) assert(type(a) == 'string') end, "cr")

-- to survive outside block
_G.showmem = showmem


assert(Cstack == Cstacklevel(),
  "should be at the same C-stack level it was when started the tests")

end   --)

local _G, showmem, print, format, clock, time, difftime,
      assert, open, warn =
      _G, showmem, print, string.format, os.clock, os.time, os.difftime,
      assert, io.open, warn

-- file with time of last performed test
local fname = T and "time-debug.txt" or "time.txt"
local lasttime

if not usertests then
  -- open file with time of last performed test
  local f = io.open(fname)
  if f then
    lasttime = assert(tonumber(f:read'a'))
    f:close();
  else   -- no such file; assume it is recording time for first time
    lasttime = nil
  end
end

-- erase (almost) all globals
print('cleaning all!!!!')
for n in pairs(_G) do
  if not ({___Glob = 1, tostring = 1})[n] then
    _G[n] = undef
  end
end


collectgarbage()
collectgarbage()
collectgarbage()
collectgarbage()
collectgarbage()
collectgarbage();showmem()

local clocktime = clock() - initclock
walltime = difftime(time(), walltime)

print(format("\n\ntotal time: %.2fs (wall time: %gs)\n", clocktime, walltime))

if not usertests then
  lasttime = lasttime or clocktime    -- if no last time, ignore difference
  -- check whether current test time differs more than 5% from last time
  local diff = (clocktime - lasttime) / lasttime
  local tolerance = 0.05    -- 5%
  if (diff >= tolerance or diff <= -tolerance) then
    warn(format("#time difference from previous test: %+.1f%%",
                  diff * 100))
  end
  assert(open(fname, "w")):write(clocktime):close()
end

print("final OK !!!")



--[[
*****************************************************************************
* Copyright (C) 1994-2016 Lua.org, PUC-Rio.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*****************************************************************************
]]

-- $Id: testes/api.lua $
-- See Copyright Notice in file all.lua

if T==nil then
    (Message or print)('\n >>> testC not active: skipping API tests <<<\n')
    return
  end
  
  local debug = require "debug"
  
  local pack = table.pack
  
  
  -- standard error message for memory errors
  local MEMERRMSG = "not enough memory"
  
  function tcheck (t1, t2)
    assert(t1.n == (t2.n or #t2) + 1)
    for i = 2, t1.n do assert(t1[i] == t2[i - 1]) end
  end
  
  
  local function checkerr (msg, f, ...)
    local stat, err = pcall(f, ...)
    assert(not stat and string.find(err, msg))
  end
  
  
  print('testing C API')
  
  a = T.testC("pushvalue R; return 1")
  assert(a == debug.getregistry())
  
  
  -- absindex
  assert(T.testC("settop 10; absindex -1; return 1") == 10)
  assert(T.testC("settop 5; absindex -5; return 1") == 1)
  assert(T.testC("settop 10; absindex 1; return 1") == 1)
  assert(T.testC("settop 10; absindex R; return 1") < -10)
  
  -- testing alignment
  a = T.d2s(12458954321123.0)
  assert(a == string.pack("d", 12458954321123.0))
  assert(T.s2d(a) == 12458954321123.0)
  
  a,b,c = T.testC("pushnum 1; pushnum 2; pushnum 3; return 2")
  assert(a == 2 and b == 3 and not c)
  
  f = T.makeCfunc("pushnum 1; pushnum 2; pushnum 3; return 2")
  a,b,c = f()
  assert(a == 2 and b == 3 and not c)
  
  -- test that all trues are equal
  a,b,c = T.testC("pushbool 1; pushbool 2; pushbool 0; return 3")
  assert(a == b and a == true and c == false)
  a,b,c = T.testC"pushbool 0; pushbool 10; pushnil;\
                        tobool -3; tobool -3; tobool -3; return 3"
  assert(a==false and b==true and c==false)
  
  
  a,b,c = T.testC("gettop; return 2", 10, 20, 30, 40)
  assert(a == 40 and b == 5 and not c)
  
  t = pack(T.testC("settop 5; return *", 2, 3))
  tcheck(t, {n=4,2,3})
  
  t = pack(T.testC("settop 0; settop 15; return 10", 3, 1, 23))
  assert(t.n == 10 and t[1] == nil and t[10] == nil)
  
  t = pack(T.testC("remove -2; return *", 2, 3, 4))
  tcheck(t, {n=2,2,4})
  
  t = pack(T.testC("insert -1; return *", 2, 3))
  tcheck(t, {n=2,2,3})
  
  t = pack(T.testC("insert 3; return *", 2, 3, 4, 5))
  tcheck(t, {n=4,2,5,3,4})
  
  t = pack(T.testC("replace 2; return *", 2, 3, 4, 5))
  tcheck(t, {n=3,5,3,4})
  
  t = pack(T.testC("replace -2; return *", 2, 3, 4, 5))
  tcheck(t, {n=3,2,3,5})
  
  t = pack(T.testC("remove 3; return *", 2, 3, 4, 5))
  tcheck(t, {n=3,2,4,5})
  
  t = pack(T.testC("copy 3 4; return *", 2, 3, 4, 5))
  tcheck(t, {n=4,2,3,3,5})
  
  t = pack(T.testC("copy -3 -1; return *", 2, 3, 4, 5))
  tcheck(t, {n=4,2,3,4,3})
  
  do   -- testing 'rotate'
    local t = {10, 20, 30, 40, 50, 60}
    for i = -6, 6 do
      local s = string.format("rotate 2 %d; return 7", i)
      local t1 = pack(T.testC(s, 10, 20, 30, 40, 50, 60))
      tcheck(t1, t)
      table.insert(t, 1, table.remove(t))
    end
  
    t = pack(T.testC("rotate -2 1; return *", 10, 20, 30, 40))
    tcheck(t, {10, 20, 40, 30})
    t = pack(T.testC("rotate -2 -1; return *", 10, 20, 30, 40))
    tcheck(t, {10, 20, 40, 30})
  
    -- some corner cases
    t = pack(T.testC("rotate -1 0; return *", 10, 20, 30, 40))
    tcheck(t, {10, 20, 30, 40})
    t = pack(T.testC("rotate -1 1; return *", 10, 20, 30, 40))
    tcheck(t, {10, 20, 30, 40})
    t = pack(T.testC("rotate 5 -1; return *", 10, 20, 30, 40))
    tcheck(t, {10, 20, 30, 40})
  end
  
  
  -- testing warnings
  T.testC([[
    warningC "#This shold be a"
    warningC " single "
    warning "warning"
    warningC "#This should be "
    warning "another one"
  ]])
  
  
  -- testing message handlers
  do
    local f = T.makeCfunc[[
      getglobal error
      pushstring bola
      pcall 1 1 1   # call 'error' with given handler
      pushstatus
      return 2     # return error message and status
    ]]
  
    local msg, st = f(string.upper)   -- function handler
    assert(st == "ERRRUN" and msg == "BOLA")
    local msg, st = f(string.len)     -- function handler
    assert(st == "ERRRUN" and msg == 4)
  
  end
  
  t = pack(T.testC("insert 3; pushvalue 3; remove 3; pushvalue 2; remove 2; \
                    insert 2; pushvalue 1; remove 1; insert 1; \
        insert -2; pushvalue -2; remove -3; return *",
        2, 3, 4, 5, 10, 40, 90))
  tcheck(t, {n=7,2,3,4,5,10,40,90})
  
  t = pack(T.testC("concat 5; return *", "alo", 2, 3, "joao", 12))
  tcheck(t, {n=1,"alo23joao12"})
  
  -- testing MULTRET
  t = pack(T.testC("call 2,-1; return *",
       function (a,b) return 1,2,3,4,a,b end, "alo", "joao"))
  tcheck(t, {n=6,1,2,3,4,"alo", "joao"})
  
  do  -- test returning more results than fit in the caller stack
    local a = {}
    for i=1,1000 do a[i] = true end; a[999] = 10
    local b = T.testC([[pcall 1 -1 0; pop 1; tostring -1; return 1]],
                      table.unpack, a)
    assert(b == "10")
  end
  
  
  -- testing globals
  _G.a = 14; _G.b = "a31"
  local a = {T.testC[[
    getglobal a;
    getglobal b;
    getglobal b;
    setglobal a;
    return *
  ]]}
  assert(a[2] == 14 and a[3] == "a31" and a[4] == nil and _G.a == "a31")
  
  
  -- testing arith
  assert(T.testC("pushnum 10; pushnum 20; arith /; return 1") == 0.5)
  assert(T.testC("pushnum 10; pushnum 20; arith -; return 1") == -10)
  assert(T.testC("pushnum 10; pushnum -20; arith *; return 1") == -200)
  assert(T.testC("pushnum 10; pushnum 3; arith ^; return 1") == 1000)
  assert(T.testC("pushnum 10; pushstring 20; arith /; return 1") == 0.5)
  assert(T.testC("pushstring 10; pushnum 20; arith -; return 1") == -10)
  assert(T.testC("pushstring 10; pushstring -20; arith *; return 1") == -200)
  assert(T.testC("pushstring 10; pushstring 3; arith ^; return 1") == 1000)
  assert(T.testC("arith /; return 1", 2, 0) == 10.0/0)
  a = T.testC("pushnum 10; pushint 3; arith \\; return 1")
  assert(a == 3.0 and math.type(a) == "float")
  a = T.testC("pushint 10; pushint 3; arith \\; return 1")
  assert(a == 3 and math.type(a) == "integer")
  a = assert(T.testC("pushint 10; pushint 3; arith +; return 1"))
  assert(a == 13 and math.type(a) == "integer")
  a = assert(T.testC("pushnum 10; pushint 3; arith +; return 1"))
  assert(a == 13 and math.type(a) == "float")
  a,b,c = T.testC([[pushnum 1;
                    pushstring 10; arith _;
                    pushstring 5; return 3]])
  assert(a == 1 and b == -10 and c == "5")
  mt = {__add = function (a,b) return setmetatable({a[1] + b[1]}, mt) end,
        __mod = function (a,b) return setmetatable({a[1] % b[1]}, mt) end,
        __unm = function (a) return setmetatable({a[1]* 2}, mt) end}
  a,b,c = setmetatable({4}, mt),
          setmetatable({8}, mt),
          setmetatable({-3}, mt)
  x,y,z = T.testC("arith +; return 2", 10, a, b)
  assert(x == 10 and y[1] == 12 and z == nil)
  assert(T.testC("arith %; return 1", a, c)[1] == 4%-3)
  assert(T.testC("arith _; arith +; arith %; return 1", b, a, c)[1] ==
                 8 % (4 + (-3)*2))
  
  -- errors in arithmetic
  checkerr("divide by zero", T.testC, "arith \\", 10, 0)
  checkerr("%%0", T.testC, "arith %", 10, 0)
  
  
  -- testing lessthan and lessequal
  assert(T.testC("compare LT 2 5, return 1", 3, 2, 2, 4, 2, 2))
  assert(T.testC("compare LE 2 5, return 1", 3, 2, 2, 4, 2, 2))
  assert(not T.testC("compare LT 3 4, return 1", 3, 2, 2, 4, 2, 2))
  assert(T.testC("compare LE 3 4, return 1", 3, 2, 2, 4, 2, 2))
  assert(T.testC("compare LT 5 2, return 1", 4, 2, 2, 3, 2, 2))
  assert(not T.testC("compare LT 2 -3, return 1", "4", "2", "2", "3", "2", "2"))
  assert(not T.testC("compare LT -3 2, return 1", "3", "2", "2", "4", "2", "2"))
  
  -- non-valid indices produce false
  assert(not T.testC("compare LT 1 4, return 1"))
  assert(not T.testC("compare LE 9 1, return 1"))
  assert(not T.testC("compare EQ 9 9, return 1"))
  
  local b = {__lt = function (a,b) return a[1] < b[1] end}
  local a1,a3,a4 = setmetatable({1}, b),
                   setmetatable({3}, b),
                   setmetatable({4}, b)
  assert(T.testC("compare LT 2 5, return 1", a3, 2, 2, a4, 2, 2))
  assert(T.testC("compare LE 2 5, return 1", a3, 2, 2, a4, 2, 2))
  assert(T.testC("compare LT 5 -6, return 1", a4, 2, 2, a3, 2, 2))
  a,b = T.testC("compare LT 5 -6, return 2", a1, 2, 2, a3, 2, 20)
  assert(a == 20 and b == false)
  a,b = T.testC("compare LE 5 -6, return 2", a1, 2, 2, a3, 2, 20)
  assert(a == 20 and b == false)
  a,b = T.testC("compare LE 5 -6, return 2", a1, 2, 2, a1, 2, 20)
  assert(a == 20 and b == true)
  
  
  do  -- testing lessthan and lessequal with metamethods
    local mt = {__lt = function (a,b) return a[1] < b[1] end,
                __le = function (a,b) return a[1] <= b[1] end,
                __eq = function (a,b) return a[1] == b[1] end}
    local function O (x)
      return setmetatable({x}, mt)
    end
  
    local a, b = T.testC("compare LT 2 3; pushint 10; return 2", O(1), O(2))
    assert(a == true and b == 10)
    local a, b = T.testC("compare LE 2 3; pushint 10; return 2", O(3), O(2))
    assert(a == false and b == 10)
    local a, b = T.testC("compare EQ 2 3; pushint 10; return 2", O(3), O(3))
    assert(a == true and b == 10)
  end
  
  -- testing length
  local t = setmetatable({x = 20}, {__len = function (t) return t.x end})
  a,b,c = T.testC([[
     len 2;
     Llen 2;
     objsize 2;
     return 3
  ]], t)
  assert(a == 20 and b == 20 and c == 0)
  
  t.x = "234"; t[1] = 20
  a,b,c = T.testC([[
     len 2;
     Llen 2;
     objsize 2;
     return 3
  ]], t)
  assert(a == "234" and b == 234 and c == 1)
  
  t.x = print; t[1] = 20
  a,c = T.testC([[
     len 2;
     objsize 2;
     return 2
  ]], t)
  assert(a == print and c == 1)
  
  
  -- testing __concat
  
  a = setmetatable({x="u"}, {__concat = function (a,b) return a.x..'.'..b.x end})
  x,y = T.testC([[
    pushnum 5
    pushvalue 2;
    pushvalue 2;
    concat 2;
    pushvalue -2;
    return 2;
  ]], a, a)
  assert(x == a..a and y == 5)
  
  -- concat with 0 elements
  assert(T.testC("concat 0; return 1") == "")
  
  -- concat with 1 element
  assert(T.testC("concat 1; return 1", "xuxu") == "xuxu")
  
  
  
  -- testing lua_is
  
  function B(x) return x and 1 or 0 end
  
  function count (x, n)
    n = n or 2
    local prog = [[
      isnumber %d;
      isstring %d;
      isfunction %d;
      iscfunction %d;
      istable %d;
      isuserdata %d;
      isnil %d;
      isnull %d;
      return 8
    ]]
    prog = string.format(prog, n, n, n, n, n, n, n, n)
    local a,b,c,d,e,f,g,h = T.testC(prog, x)
    return B(a)+B(b)+B(c)+B(d)+B(e)+B(f)+B(g)+(100*B(h))
  end
  
  assert(count(3) == 2)
  assert(count('alo') == 1)
  assert(count('32') == 2)
  assert(count({}) == 1)
  assert(count(print) == 2)
  assert(count(function () end) == 1)
  assert(count(nil) == 1)
  assert(count(io.stdin) == 1)
  assert(count(nil, 15) == 100)
  
  
  -- testing lua_to...
  
  function to (s, x, n)
    n = n or 2
    return T.testC(string.format("%s %d; return 1", s, n), x)
  end
  
  local null = T.pushuserdata(0)
  local hfunc = string.gmatch("", "")    -- a "heavy C function" (with upvalues)
  assert(debug.getupvalue(hfunc, 1))
  assert(to("tostring", {}) == nil)
  assert(to("tostring", "alo") == "alo")
  assert(to("tostring", 12) == "12")
  assert(to("tostring", 12, 3) == nil)
  assert(to("objsize", {}) == 0)
  assert(to("objsize", {1,2,3}) == 3)
  assert(to("objsize", "alo\0\0a") == 6)
  assert(to("objsize", T.newuserdata(0)) == 0)
  assert(to("objsize", T.newuserdata(101)) == 101)
  assert(to("objsize", 124) == 0)
  assert(to("objsize", true) == 0)
  assert(to("tonumber", {}) == 0)
  assert(to("tonumber", "12") == 12)
  assert(to("tonumber", "s2") == 0)
  assert(to("tonumber", 1, 20) == 0)
  assert(to("topointer", 10) == null)
  assert(to("topointer", true) == null)
  assert(to("topointer", nil) == null)
  assert(to("topointer", "abc") ~= null)
  assert(to("topointer", string.rep("x", 10)) ==
         to("topointer", string.rep("x", 10)))    -- short strings
  do    -- long strings
    local s1 = string.rep("x", 300)
    local s2 = string.rep("x", 300)
    assert(to("topointer", s1) ~= to("topointer", s2))
  end
  assert(to("topointer", T.pushuserdata(20)) ~= null)
  assert(to("topointer", io.read) ~= null)           -- light C function
  assert(to("topointer", hfunc) ~= null)        -- "heavy" C function
  assert(to("topointer", function () end) ~= null)   -- Lua function
  assert(to("topointer", io.stdin) ~= null)   -- full userdata
  assert(to("func2num", 20) == 0)
  assert(to("func2num", T.pushuserdata(10)) == 0)
  assert(to("func2num", io.read) ~= 0)     -- light C function
  assert(to("func2num", hfunc) ~= 0)  -- "heavy" C function (with upvalue)
  a = to("tocfunction", math.deg)
  assert(a(3) == math.deg(3) and a == math.deg)
  
  
  print("testing panic function")
  do
    -- trivial error
    assert(T.checkpanic("pushstring hi; error") == "hi")
  
    -- using the stack inside panic
    assert(T.checkpanic("pushstring hi; error;",
      [[checkstack 5 XX
        pushstring ' alo'
        pushstring ' mundo'
        concat 3]]) == "hi alo mundo")
  
    -- "argerror" without frames
    assert(T.checkpanic("loadstring 4") ==
        "bad argument #4 (string expected, got no value)")
  
  
    -- memory error
    T.totalmem(T.totalmem()+10000)   -- set low memory limit (+10k)
    assert(T.checkpanic("newuserdata 20000") == MEMERRMSG)
    T.totalmem(0)          -- restore high limit
  
    -- stack error
    if not _soft then
      local msg = T.checkpanic[[
        pushstring "function f() f() end"
        loadstring -1; call 0 0
        getglobal f; call 0 0
      ]]
      assert(string.find(msg, "stack overflow"))
    end
  
    -- exit in panic still close to-be-closed variables
    assert(T.checkpanic([[
      pushstring "return {__close = function () Y = 'ho'; end}"
      newtable
      loadstring -2
      call 0 1
      setmetatable -2
      toclose -1
      pushstring "hi"
      error
    ]],
    [[
      getglobal Y
      concat 2         # concat original error with global Y
    ]]) == "hiho")
  
  
  end
  
  -- testing deep C stack
  if not _soft then
    print("testing stack overflow")
    collectgarbage("stop")
    checkerr("XXXX", T.testC, "checkstack 1000023 XXXX")   -- too deep
    -- too deep (with no message)
    checkerr("^stack overflow$", T.testC, "checkstack 1000023 ''")
    local s = string.rep("pushnil;checkstack 1 XX;", 1000000)
    checkerr("overflow", T.testC, s)
    collectgarbage("restart")
    print'+'
  end
  
  local lim = _soft and 500 or 12000
  local prog = {"checkstack " .. (lim * 2 + 100) .. "msg", "newtable"}
  for i = 1,lim do
    prog[#prog + 1] = "pushnum " .. i
    prog[#prog + 1] = "pushnum " .. i * 10
  end
  
  prog[#prog + 1] = "rawgeti R 2"   -- get global table in registry
  prog[#prog + 1] = "insert " .. -(2*lim + 2)
  
  for i = 1,lim do
    prog[#prog + 1] = "settable " .. -(2*(lim - i + 1) + 1)
  end
  
  prog[#prog + 1] = "return 2"
  
  prog = table.concat(prog, ";")
  local g, t = T.testC(prog)
  assert(g == _G)
  for i = 1,lim do assert(t[i] == i*10); t[i] = undef end
  assert(next(t) == nil)
  prog, g, t = nil
  
  -- testing errors
  
  a = T.testC([[
    loadstring 2; pcall 0 1 0;
    pushvalue 3; insert -2; pcall 1 1 0;
    pcall 0 0 0;
    return 1
  ]], "x=150", function (a) assert(a==nil); return 3 end)
  
  assert(type(a) == 'string' and x == 150)
  
  function check3(p, ...)
    local arg = {...}
    assert(#arg == 3)
    assert(string.find(arg[3], p))
  end
  check3(":1:", T.testC("loadstring 2; return *", "x="))
  check3("%.", T.testC("loadfile 2; return *", "."))
  check3("xxxx", T.testC("loadfile 2; return *", "xxxx"))
  
  -- test errors in non protected threads
  function checkerrnopro (code, msg)
    local th = coroutine.create(function () end)  -- create new thread
    local stt, err = pcall(T.testC, th, code)   -- run code there
    assert(not stt and string.find(err, msg))
  end
  
  if not _soft then
    collectgarbage("stop")   -- avoid __gc with full stack
    checkerrnopro("pushnum 3; call 0 0", "attempt to call")
    print"testing stack overflow in unprotected thread"
    function f () f() end
    checkerrnopro("getglobal 'f'; call 0 0;", "stack overflow")
    collectgarbage("restart")
  end
  print"+"
  
  
  -- testing table access
  
  do   -- getp/setp
    local a = {}
    local a1 = T.testC("rawsetp 2 1; return 1", a, 20)
    assert(a == a1)
    assert(a[T.pushuserdata(1)] == 20)
    local a1, res = T.testC("rawgetp -1 1; return 2", a)
    assert(a == a1 and res == 20)
  end
  
  
  do  -- using the table itself as index
    local a = {}
    a[a] = 10
    local prog = "gettable -1; return *"
    local res = {T.testC(prog, a)}
    assert(#res == 2 and res[1] == prog and res[2] == 10)
  
    local prog = "settable -2; return *"
    local res = {T.testC(prog, a, 20)}
    assert(a[a] == 20)
    assert(#res == 1 and res[1] == prog)
  
    -- raw
    a[a] = 10
    local prog = "rawget -1; return *"
    local res = {T.testC(prog, a)}
    assert(#res == 2 and res[1] == prog and res[2] == 10)
  
    local prog = "rawset -2; return *"
    local res = {T.testC(prog, a, 20)}
    assert(a[a] == 20)
    assert(#res == 1 and res[1] == prog)
  
    -- using the table as the value to set
    local prog = "rawset -1; return *"
    local res = {T.testC(prog, 30, a)}
    assert(a[30] == a)
    assert(#res == 1 and res[1] == prog)
  
    local prog = "settable -1; return *"
    local res = {T.testC(prog, 40, a)}
    assert(a[40] == a)
    assert(#res == 1 and res[1] == prog)
  
    local prog = "rawseti -1 100; return *"
    local res = {T.testC(prog, a)}
    assert(a[100] == a)
    assert(#res == 1 and res[1] == prog)
  
    local prog = "seti -1 200; return *"
    local res = {T.testC(prog, a)}
    assert(a[200] == a)
    assert(#res == 1 and res[1] == prog)
  end
  
  a = {x=0, y=12}
  x, y = T.testC("gettable 2; pushvalue 4; gettable 2; return 2",
                  a, 3, "y", 4, "x")
  assert(x == 0 and y == 12)
  T.testC("settable -5", a, 3, 4, "x", 15)
  assert(a.x == 15)
  a[a] = print
  x = T.testC("gettable 2; return 1", a)  -- table and key are the same object!
  assert(x == print)
  T.testC("settable 2", a, "x")    -- table and key are the same object!
  assert(a[a] == "x")
  
  b = setmetatable({p = a}, {})
  getmetatable(b).__index = function (t, i) return t.p[i] end
  k, x = T.testC("gettable 3, return 2", 4, b, 20, 35, "x")
  assert(x == 15 and k == 35)
  k = T.testC("getfield 2 y, return 1", b)
  assert(k == 12)
  getmetatable(b).__index = function (t, i) return a[i] end
  getmetatable(b).__newindex = function (t, i,v ) a[i] = v end
  y = T.testC("insert 2; gettable -5; return 1", 2, 3, 4, "y", b)
  assert(y == 12)
  k = T.testC("settable -5, return 1", b, 3, 4, "x", 16)
  assert(a.x == 16 and k == 4)
  a[b] = 'xuxu'
  y = T.testC("gettable 2, return 1", b)
  assert(y == 'xuxu')
  T.testC("settable 2", b, 19)
  assert(a[b] == 19)
  
  --
  do   -- testing getfield/setfield with long keys
    local t = {_012345678901234567890123456789012345678901234567890123456789 = 32}
    local a = T.testC([[
      getfield 2 _012345678901234567890123456789012345678901234567890123456789
      return 1
    ]], t)
    assert(a == 32)
    local a = T.testC([[
      pushnum 33
      setglobal _012345678901234567890123456789012345678901234567890123456789
    ]])
    assert(_012345678901234567890123456789012345678901234567890123456789 == 33)
    _012345678901234567890123456789012345678901234567890123456789 = nil
  end
  
  -- testing next
  a = {}
  t = pack(T.testC("next; return *", a, nil))
  tcheck(t, {n=1,a})
  a = {a=3}
  t = pack(T.testC("next; return *", a, nil))
  tcheck(t, {n=3,a,'a',3})
  t = pack(T.testC("next; pop 1; next; return *", a, nil))
  tcheck(t, {n=1,a})
  
  
  
  -- testing upvalues
  
  do
    local A = T.testC[[ pushnum 10; pushnum 20; pushcclosure 2; return 1]]
    t, b, c = A([[pushvalue U0; pushvalue U1; pushvalue U2; return 3]])
    assert(b == 10 and c == 20 and type(t) == 'table')
    a, b = A([[tostring U3; tonumber U4; return 2]])
    assert(a == nil and b == 0)
    A([[pushnum 100; pushnum 200; replace U2; replace U1]])
    b, c = A([[pushvalue U1; pushvalue U2; return 2]])
    assert(b == 100 and c == 200)
    A([[replace U2; replace U1]], {x=1}, {x=2})
    b, c = A([[pushvalue U1; pushvalue U2; return 2]])
    assert(b.x == 1 and c.x == 2)
    T.checkmemory()
  end
  
  
  -- testing absent upvalues from C-function pointers
  assert(T.testC[[isnull U1; return 1]] == true)
  assert(T.testC[[isnull U100; return 1]] == true)
  assert(T.testC[[pushvalue U1; return 1]] == nil)
  
  local f = T.testC[[ pushnum 10; pushnum 20; pushcclosure 2; return 1]]
  assert(T.upvalue(f, 1) == 10 and
         T.upvalue(f, 2) == 20 and
         T.upvalue(f, 3) == nil)
  T.upvalue(f, 2, "xuxu")
  assert(T.upvalue(f, 2) == "xuxu")
  
  
  -- large closures
  do
    local A = "checkstack 300 msg;" ..
              string.rep("pushnum 10;", 255) ..
              "pushcclosure 255; return 1"
    A = T.testC(A)
    for i=1,255 do
      assert(A(("pushvalue U%d; return 1"):format(i)) == 10)
    end
    assert(A("isnull U256; return 1"))
    assert(not A("isnil U256; return 1"))
  end
  
  
  
  -- testing get/setuservalue
  -- bug in 5.1.2
  checkerr("got number", debug.setuservalue, 3, {})
  checkerr("got nil", debug.setuservalue, nil, {})
  checkerr("got light userdata", debug.setuservalue, T.pushuserdata(1), {})
  
  -- testing multiple user values
  local b = T.newuserdata(0, 10)
  for i = 1, 10 do
    local v, p = debug.getuservalue(b, i)
    assert(v == nil and p)
  end
  do   -- indices out of range
    local v, p = debug.getuservalue(b, -2)
    assert(v == nil and not p)
    local v, p = debug.getuservalue(b, 11)
    assert(v == nil and not p)
  end
  local t = {true, false, 4.56, print, {}, b, "XYZ"}
  for k, v in ipairs(t) do
    debug.setuservalue(b, v, k)
  end
  for k, v in ipairs(t) do
    local v1, p = debug.getuservalue(b, k)
    assert(v1 == v and p)
  end
  
  assert(not debug.getuservalue(4))
  
  debug.setuservalue(b, function () return 10 end, 10)
  collectgarbage()   -- function should not be collected
  assert(debug.getuservalue(b, 10)() == 10)
  
  debug.setuservalue(b, 134)
  collectgarbage()   -- number should not be a problem for collector
  assert(debug.getuservalue(b) == 134)
  
  
  -- test barrier for uservalues
  do
    local oldmode = collectgarbage("incremental")
    T.gcstate("atomic")
    assert(T.gccolor(b) == "black")
    debug.setuservalue(b, {x = 100})
    T.gcstate("pause")  -- complete collection
    assert(debug.getuservalue(b).x == 100)  -- uvalue should be there
    collectgarbage(oldmode)
  end
  
  -- long chain of userdata
  for i = 1, 1000 do
    local bb = T.newuserdata(0, 1)
    debug.setuservalue(bb, b)
    b = bb
  end
  collectgarbage()     -- nothing should not be collected
  for i = 1, 1000 do
    b = debug.getuservalue(b)
  end
  assert(debug.getuservalue(b).x == 100)
  b = nil
  
  
  -- testing locks (refs)
  
  -- reuse of references
  local i = T.ref{}
  T.unref(i)
  assert(T.ref{} == i)
  
  Arr = {}
  Lim = 100
  for i=1,Lim do   -- lock many objects
    Arr[i] = T.ref({})
  end
  
  assert(T.ref(nil) == -1 and T.getref(-1) == nil)
  T.unref(-1); T.unref(-1)
  
  for i=1,Lim do   -- unlock all them
    T.unref(Arr[i])
  end
  
  function printlocks ()
    local f = T.makeCfunc("gettable R; return 1")
    local n = f("n")
    print("n", n)
    for i=0,n do
      print(i, f(i))
    end
  end
  
  
  for i=1,Lim do   -- lock many objects
    Arr[i] = T.ref({})
  end
  
  for i=1,Lim,2 do   -- unlock half of them
    T.unref(Arr[i])
  end
  
  assert(type(T.getref(Arr[2])) == 'table')
  
  
  assert(T.getref(-1) == nil)
  
  
  a = T.ref({})
  
  collectgarbage()
  
  assert(type(T.getref(a)) == 'table')
  
  
  -- colect in cl the `val' of all collected userdata
  tt = {}
  cl = {n=0}
  A = nil; B = nil
  local F
  F = function (x)
    local udval = T.udataval(x)
    table.insert(cl, udval)
    local d = T.newuserdata(100)   -- create garbage
    d = nil
    assert(debug.getmetatable(x).__gc == F)
    assert(load("table.insert({}, {})"))()   -- create more garbage
    collectgarbage()   -- force a GC during GC
    assert(debug.getmetatable(x).__gc == F)  -- previous GC did not mess this?
    local dummy = {}    -- create more garbage during GC
    if A ~= nil then
      assert(type(A) == "userdata")
      assert(T.udataval(A) == B)
      debug.getmetatable(A)    -- just access it
    end
    A = x   -- ressucita userdata
    B = udval
    return 1,2,3
  end
  tt.__gc = F
  
  -- test whether udate collection frees memory in the right time
  do
    collectgarbage();
    collectgarbage();
    local x = collectgarbage("count");
    local a = T.newuserdata(5001)
    assert(T.testC("objsize 2; return 1", a) == 5001)
    assert(collectgarbage("count") >= x+4)
    a = nil
    collectgarbage();
    assert(collectgarbage("count") <= x+1)
    -- udata without finalizer
    x = collectgarbage("count")
    collectgarbage("stop")
    for i=1,1000 do T.newuserdata(0) end
    assert(collectgarbage("count") > x+10)
    collectgarbage()
    assert(collectgarbage("count") <= x+1)
    -- udata with finalizer
    collectgarbage()
    x = collectgarbage("count")
    collectgarbage("stop")
    a = {__gc = function () end}
    for i=1,1000 do debug.setmetatable(T.newuserdata(0), a) end
    assert(collectgarbage("count") >= x+10)
    collectgarbage()  -- this collection only calls TM, without freeing memory
    assert(collectgarbage("count") >= x+10)
    collectgarbage()  -- now frees memory
    assert(collectgarbage("count") <= x+1)
    collectgarbage("restart")
  end
  
  
  collectgarbage("stop")
  
  -- create 3 userdatas with tag `tt'
  a = T.newuserdata(0); debug.setmetatable(a, tt); na = T.udataval(a)
  b = T.newuserdata(0); debug.setmetatable(b, tt); nb = T.udataval(b)
  c = T.newuserdata(0); debug.setmetatable(c, tt); nc = T.udataval(c)
  
  -- create userdata without meta table
  x = T.newuserdata(4)
  y = T.newuserdata(0)
  
  checkerr("FILE%* expected, got userdata", io.input, a)
  checkerr("FILE%* expected, got userdata", io.input, x)
  
  assert(debug.getmetatable(x) == nil and debug.getmetatable(y) == nil)
  
  d=T.ref(a);
  e=T.ref(b);
  f=T.ref(c);
  t = {T.getref(d), T.getref(e), T.getref(f)}
  assert(t[1] == a and t[2] == b and t[3] == c)
  
  t=nil; a=nil; c=nil;
  T.unref(e); T.unref(f)
  
  collectgarbage()
  
  -- check that unref objects have been collected
  assert(#cl == 1 and cl[1] == nc)
  
  x = T.getref(d)
  assert(type(x) == 'userdata' and debug.getmetatable(x) == tt)
  x =nil
  tt.b = b  -- create cycle
  tt=nil    -- frees tt for GC
  A = nil
  b = nil
  T.unref(d);
  n5 = T.newuserdata(0)
  debug.setmetatable(n5, {__gc=F})
  n5 = T.udataval(n5)
  collectgarbage()
  assert(#cl == 4)
  -- check order of collection
  assert(cl[2] == n5 and cl[3] == nb and cl[4] == na)
  
  collectgarbage"restart"
  
  
  a, na = {}, {}
  for i=30,1,-1 do
    a[i] = T.newuserdata(0)
    debug.setmetatable(a[i], {__gc=F})
    na[i] = T.udataval(a[i])
  end
  cl = {}
  a = nil; collectgarbage()
  assert(#cl == 30)
  for i=1,30 do assert(cl[i] == na[i]) end
  na = nil
  
  
  for i=2,Lim,2 do   -- unlock the other half
    T.unref(Arr[i])
  end
  
  x = T.newuserdata(41); debug.setmetatable(x, {__gc=F})
  assert(T.testC("objsize 2; return 1", x) == 41)
  cl = {}
  a = {[x] = 1}
  x = T.udataval(x)
  collectgarbage()
  -- old `x' cannot be collected (`a' still uses it)
  assert(#cl == 0)
  for n in pairs(a) do a[n] = undef end
  collectgarbage()
  assert(#cl == 1 and cl[1] == x)   -- old `x' must be collected
  
  -- testing lua_equal
  assert(T.testC("compare EQ 2 4; return 1", print, 1, print, 20))
  assert(T.testC("compare EQ 3 2; return 1", 'alo', "alo"))
  assert(T.testC("compare EQ 2 3; return 1", nil, nil))
  assert(not T.testC("compare EQ 2 3; return 1", {}, {}))
  assert(not T.testC("compare EQ 2 3; return 1"))
  assert(not T.testC("compare EQ 2 3; return 1", 3))
  
  -- testing lua_equal with fallbacks
  do
    local map = {}
    local t = {__eq = function (a,b) return map[a] == map[b] end}
    local function f(x)
      local u = T.newuserdata(0)
      debug.setmetatable(u, t)
      map[u] = x
      return u
    end
    assert(f(10) == f(10))
    assert(f(10) ~= f(11))
    assert(T.testC("compare EQ 2 3; return 1", f(10), f(10)))
    assert(not T.testC("compare EQ 2 3; return 1", f(10), f(20)))
    t.__eq = nil
    assert(f(10) ~= f(10))
  end
  
  print'+'
  
  
  
  -- testing changing hooks during hooks
  _G.t = {}
  T.sethook([[
    # set a line hook after 3 count hooks
    sethook 4 0 '
      getglobal t;
      pushvalue -3; append -2
      pushvalue -2; append -2
    ']], "c", 3)
  local a = 1   -- counting
  a = 1   -- counting
  a = 1   -- count hook (set line hook)
  a = 1   -- line hook
  a = 1   -- line hook
  debug.sethook()
  t = _G.t
  assert(t[1] == "line")
  line = t[2]
  assert(t[3] == "line" and t[4] == line + 1)
  assert(t[5] == "line" and t[6] == line + 2)
  assert(t[7] == nil)
  
  
  -------------------------------------------------------------------------
  do   -- testing errors during GC
    warn("@off")
    collectgarbage("stop")
    local a = {}
    for i=1,20 do
      a[i] = T.newuserdata(i)   -- creates several udata
    end
    for i=1,20,2 do   -- mark half of them to raise errors during GC
      debug.setmetatable(a[i],
        {__gc = function (x) error("@expected error in gc") end})
    end
    for i=2,20,2 do   -- mark the other half to count and to create more garbage
      debug.setmetatable(a[i], {__gc = function (x) load("A=A+1")() end})
    end
    a = nil
    _G.A = 0
    collectgarbage()
    assert(A == 10)  -- number of normal collections
    collectgarbage("restart")
    warn("@on")
  end
  -------------------------------------------------------------------------
  -- test for userdata vals
  do
    local a = {}; local lim = 30
    for i=0,lim do a[i] = T.pushuserdata(i) end
    for i=0,lim do assert(T.udataval(a[i]) == i) end
    for i=0,lim do assert(T.pushuserdata(i) == a[i]) end
    for i=0,lim do a[a[i]] = i end
    for i=0,lim do a[T.pushuserdata(i)] = i end
    assert(type(tostring(a[1])) == "string")
  end
  
  
  -------------------------------------------------------------------------
  -- testing multiple states
  T.closestate(T.newstate());
  L1 = T.newstate()
  assert(L1)
  
  assert(T.doremote(L1, "X='a'; return 'a'") == 'a')
  
  
  assert(#pack(T.doremote(L1, "function f () return 'alo', 3 end; f()")) == 0)
  
  a, b = T.doremote(L1, "return f()")
  assert(a == 'alo' and b == '3')
  
  T.doremote(L1, "_ERRORMESSAGE = nil")
  -- error: `sin' is not defined
  a, _, b = T.doremote(L1, "return sin(1)")
  assert(a == nil and b == 2)   -- 2 == run-time error
  
  -- error: syntax error
  a, b, c = T.doremote(L1, "return a+")
  assert(a == nil and c == 3 and type(b) == "string")   -- 3 == syntax error
  
  T.loadlib(L1)
  a, b, c = T.doremote(L1, [[
    string = require'string'
    a = require'_G'; assert(a == _G and require("_G") == a)
    io = require'io'; assert(type(io.read) == "function")
    assert(require("io") == io)
    a = require'table'; assert(type(a.insert) == "function")
    a = require'debug'; assert(type(a.getlocal) == "function")
    a = require'math'; assert(type(a.sin) == "function")
    return string.sub('okinama', 1, 2)
  ]])
  assert(a == "ok")
  
  T.closestate(L1);
  
  
  L1 = T.newstate()
  T.loadlib(L1)
  T.doremote(L1, "a = {}")
  T.testC(L1, [[getglobal "a"; pushstring "x"; pushint 1;
               settable -3]])
  assert(T.doremote(L1, "return a.x") == "1")
  
  T.closestate(L1)
  
  L1 = nil
  
  print('+')
  -------------------------------------------------------------------------
  -- testing to-be-closed variables
  -------------------------------------------------------------------------
  print"testing to-be-closed variables"
  
  do
    local openresource = {}
  
    local function newresource ()
      local x = setmetatable({10}, {__close = function(y)
        assert(openresource[#openresource] == y)
        openresource[#openresource] = nil
        y[1] = y[1] + 1
      end})
      openresource[#openresource + 1] = x
      return x
    end
  
    local a, b = T.testC([[
      call 0 1   # create resource
      pushnil
      toclose -2  # mark call result to be closed
      toclose -1  # mark nil to be closed (will be ignored)
      return 2
    ]], newresource)
    assert(a[1] == 11 and b == nil)
    assert(#openresource == 0)    -- was closed
  
    -- repeat the test, but calling function in a 'multret' context
    local a = {T.testC([[
      call 0 1   # create resource
      toclose 2 # mark it to be closed
      return 2
    ]], newresource)}
    assert(type(a[1]) == "string" and a[2][1] == 11)
    assert(#openresource == 0)    -- was closed
  
    -- closing by error
    local a, b = pcall(T.makeCfunc[[
      call 0 1   # create resource
      toclose -1 # mark it to be closed
      error       # resource is the error object
    ]], newresource)
    assert(a == false and b[1] == 11)
    assert(#openresource == 0)    -- was closed
  
    -- non-closable value
    local a, b = pcall(T.makeCfunc[[
      newtable   # create non-closable object
      toclose -1 # mark it to be closed (should raise an error)
      abort  # will not be executed
    ]])
    assert(a == false and
      string.find(b, "non%-closable value"))
  
    local function check (n)
      assert(#openresource == n)
    end
  
    -- closing resources with 'closeslot'
    _ENV.xxx = true
    local a = T.testC([[
      pushvalue 2  # stack: S, NR, CH, NR
      call 0 1   # create resource; stack: S, NR, CH, R
      toclose -1 # mark it to be closed
      pushvalue 2  #  stack: S, NR, CH, R, NR
      call 0 1   # create another resource; stack: S, NR, CH, R, R
      toclose -1 # mark it to be closed
      pushvalue 3  # stack: S, NR, CH, R, R, CH
      pushint 2   # there should be two open resources
      call 1 0  #  stack: S, NR, CH, R, R
      closeslot -1   # close second resource
      pushvalue 3  # stack: S, NR, CH, R, R, CH
      pushint 1   # there should be one open resource
      call 1 0  # stack: S, NR, CH, R, R
      closeslot 4
      setglobal "xxx"  # previous op. erased the slot
      pop 1       # pop other resource from the stack
      pushint *
      return 1    # return stack size
    ]], newresource, check)
    assert(a == 3 and _ENV.xxx == nil)   -- no extra items left in the stack
  
    -- closing resources with 'pop'
    local a = T.testC([[
      pushvalue 2  # stack: S, NR, CH, NR
      call 0 1   # create resource; stack: S, NR, CH, R
      toclose -1 # mark it to be closed
      pushvalue 2  #  stack: S, NR, CH, R, NR
      call 0 1   # create another resource; stack: S, NR, CH, R, R
      toclose -1 # mark it to be closed
      pushvalue 3  # stack: S, NR, CH, R, R, CH
      pushint 2   # there should be two open resources
      call 1 0  #  stack: S, NR, CH, R, R
      pop 1   # pop second resource
      pushvalue 3  # stack: S, NR, CH, R, CH
      pushint 1   # there should be one open resource
      call 1 0  # stack: S, NR, CH, R
      pop 1       # pop other resource from the stack
      pushvalue 3  # stack: S, NR, CH, CH
      pushint 0   # there should be no open resources
      call 1 0  # stack: S, NR, CH
      pushint *
      return 1    # return stack size
    ]], newresource, check)
    assert(a == 3)   -- no extra items left in the stack
  
    -- non-closable value
    local a, b = pcall(T.makeCfunc[[
      pushint 32
      toclose -1
    ]])
    assert(not a and string.find(b, "(C temporary)"))
  
  end
  
  
  --[[
  ** {==================================================================
  ** Testing memory limits
  ** ===================================================================
  --]]
  
  print("memory-allocation errors")
  
  checkerr("block too big", T.newuserdata, math.maxinteger)
  collectgarbage()
  local f = load"local a={}; for i=1,100000 do a[i]=i end"
  T.alloccount(10)
  checkerr(MEMERRMSG, f)
  T.alloccount()          -- remove limit
  
  
  -- test memory errors; increase limit for maximum memory by steps,
  -- o that we get memory errors in all allocations of a given
  -- task, until there is enough memory to complete the task without
  -- errors.
  function testbytes (s, f)
    collectgarbage()
    local M = T.totalmem()
    local oldM = M
    local a,b = nil
    while true do
      collectgarbage(); collectgarbage()
      T.totalmem(M)
      a, b = T.testC("pcall 0 1 0; pushstatus; return 2", f)
      T.totalmem(0)  -- remove limit
      if a and b == "OK" then break end       -- stop when no more errors
      if b ~= "OK" and b ~= MEMERRMSG then    -- not a memory error?
        error(a, 0)   -- propagate it
      end
      M = M + 7   -- increase memory limit
    end
    print(string.format("minimum memory for %s: %d bytes", s, M - oldM))
    return a
  end
  
  -- test memory errors; increase limit for number of allocations one
  -- by one, so that we get memory errors in all allocations of a given
  -- task, until there is enough allocations to complete the task without
  -- errors.
  
  function testalloc (s, f)
    collectgarbage()
    local M = 0
    local a,b = nil
    while true do
      collectgarbage(); collectgarbage()
      T.alloccount(M)
      a, b = T.testC("pcall 0 1 0; pushstatus; return 2", f)
      T.alloccount()  -- remove limit
      if a and b == "OK" then break end       -- stop when no more errors
      if b ~= "OK" and b ~= MEMERRMSG then    -- not a memory error?
        error(a, 0)   -- propagate it
      end
      M = M + 1   -- increase allocation limit
    end
    print(string.format("minimum allocations for %s: %d allocations", s, M))
    return a
  end
  
  
  local function testamem (s, f)
    testalloc(s, f)
    return testbytes(s, f)
  end
  
  
  -- doing nothing
  b = testamem("doing nothing", function () return 10 end)
  assert(b == 10)
  
  -- testing memory errors when creating a new state
  
  testamem("state creation", function ()
    local st = T.newstate()
    if st then T.closestate(st) end   -- close new state
    return st
  end)
  
  testamem("empty-table creation", function ()
    return {}
  end)
  
  testamem("string creation", function ()
    return "XXX" .. "YYY"
  end)
  
  testamem("coroutine creation", function()
             return coroutine.create(print)
  end)
  
  
  -- testing to-be-closed variables
  testamem("to-be-closed variables", function()
    local flag
    do
      local x <close> =
                setmetatable({}, {__close = function () flag = true end})
      flag = false
      local x = {}
    end
    return flag
  end)
  
  
  -- testing threads
  
  -- get main thread from registry (at index LUA_RIDX_MAINTHREAD == 1)
  mt = T.testC("rawgeti R 1; return 1")
  assert(type(mt) == "thread" and coroutine.running() == mt)
  
  
  
  function expand (n,s)
    if n==0 then return "" end
    local e = string.rep("=", n)
    return string.format("T.doonnewstack([%s[ %s;\n collectgarbage(); %s]%s])\n",
                                e, s, expand(n-1,s), e)
  end
  
  G=0; collectgarbage(); a =collectgarbage("count")
  load(expand(20,"G=G+1"))()
  assert(G==20); collectgarbage();  -- assert(gcinfo() <= a+1)
  
  testamem("running code on new thread", function ()
    return T.doonnewstack("x=1") == 0  -- try to create thread
  end)
  
  
  -- testing memory x compiler
  
  testamem("loadstring", function ()
    return load("x=1")  -- try to do load a string
  end)
  
  
  local testprog = [[
  local function foo () return end
  local t = {"x"}
  a = "aaa"
  for i = 1, #t do a=a..t[i] end
  return true
  ]]
  
  -- testing memory x dofile
  _G.a = nil
  local t =os.tmpname()
  local f = assert(io.open(t, "w"))
  f:write(testprog)
  f:close()
  testamem("dofile", function ()
    local a = loadfile(t)
    return a and a()
  end)
  assert(os.remove(t))
  assert(_G.a == "aaax")
  
  
  -- other generic tests
  
  testamem("gsub", function ()
    local a, b = string.gsub("alo alo", "(a)", function (x) return x..'b' end)
    return (a == 'ablo ablo')
  end)
  
  testamem("dump/undump", function ()
    local a = load(testprog)
    local b = a and string.dump(a)
    a = b and load(b)
    return a and a()
  end)
  
  local t = os.tmpname()
  testamem("file creation", function ()
    local f = assert(io.open(t, 'w'))
    assert (not io.open"nomenaoexistente")
    io.close(f);
    return not loadfile'nomenaoexistente'
  end)
  assert(os.remove(t))
  
  testamem("table creation", function ()
    local a, lim = {}, 10
    for i=1,lim do a[i] = i; a[i..'a'] = {} end
    return (type(a[lim..'a']) == 'table' and a[lim] == lim)
  end)
  
  testamem("constructors", function ()
    local a = {10, 20, 30, 40, 50; a=1, b=2, c=3, d=4, e=5}
    return (type(a) == 'table' and a.e == 5)
  end)
  
  local a = 1
  close = nil
  testamem("closure creation", function ()
    function close (b)
     return function (x) return b + x end
    end
    return (close(2)(4) == 6)
  end)
  
  testamem("using coroutines", function ()
    local a = coroutine.wrap(function ()
                coroutine.yield(string.rep("a", 10))
                return {}
              end)
    assert(string.len(a()) == 10)
    return a()
  end)
  
  do   -- auxiliary buffer
    local lim = 100
    local a = {}; for i = 1, lim do a[i] = "01234567890123456789" end
    testamem("auxiliary buffer", function ()
      return (#table.concat(a, ",") == 20*lim + lim - 1)
    end)
  end
  
  testamem("growing stack", function ()
    local function foo (n)
      if n == 0 then return 1 else return 1 + foo(n - 1) end
    end
    return foo(100)
  end)
  
  -- }==================================================================
  
  
  do   -- testing failing in 'lua_checkstack'
    local res = T.testC([[rawcheckstack 500000; return 1]])
    assert(res == false)
    local L = T.newstate()
    T.alloccount(0)   -- will be unable to reallocate the stack
    res = T.testC(L, [[rawcheckstack 5000; return 1]])
    T.alloccount()
    T.closestate(L)
    assert(res == false)
  end
  
  do   -- closing state with no extra memory
    local L = T.newstate()
    T.alloccount(0)
    T.closestate(L)
    T.alloccount()
  end
  
  do   -- garbage collection with no extra memory
    local L = T.newstate()
    T.loadlib(L)
    local res = (T.doremote(L, [[
      _ENV = require"_G"
      local T = require"T"
      local a = {}
      for i = 1, 1000 do a[i] = 'i' .. i end    -- grow string table
      local stsize, stuse = T.querystr()
      assert(stuse > 1000)
      local function foo (n)
        if n > 0 then foo(n - 1) end
      end
      foo(180)    -- grow stack
      local _, stksize = T.stacklevel()
      assert(stksize > 180)
      a = nil
      T.alloccount(0)
      collectgarbage()
      T.alloccount()
      -- stack and string table could not be reallocated,
      -- so they kept their sizes (without errors)
      assert(select(2, T.stacklevel()) == stksize)
      assert(T.querystr() == stsize)
      return 'ok'
    ]]))
    assert(res == 'ok')
    T.closestate(L)
  end
  
  print'+'
  
  -- testing some auxlib functions
  local function gsub (a, b, c)
    a, b = T.testC("gsub 2 3 4; gettop; return 2", a, b, c)
    assert(b == 5)
    return a
  end
  
  assert(gsub("alo.alo.uhuh.", ".", "//") == "alo//alo//uhuh//")
  assert(gsub("alo.alo.uhuh.", "alo", "//") == "//.//.uhuh.")
  assert(gsub("", "alo", "//") == "")
  assert(gsub("...", ".", "/.") == "/././.")
  assert(gsub("...", "...", "") == "")
  
  
  -- testing luaL_newmetatable
  local mt_xuxu, res, top = T.testC("newmetatable xuxu; gettop; return 3")
  assert(type(mt_xuxu) == "table" and res and top == 3)
  local d, res, top = T.testC("newmetatable xuxu; gettop; return 3")
  assert(mt_xuxu == d and not res and top == 3)
  d, res, top = T.testC("newmetatable xuxu1; gettop; return 3")
  assert(mt_xuxu ~= d and res and top == 3)
  
  x = T.newuserdata(0);
  y = T.newuserdata(0);
  T.testC("pushstring xuxu; gettable R; setmetatable 2", x)
  assert(getmetatable(x) == mt_xuxu)
  
  -- testing luaL_testudata
  -- correct metatable
  local res1, res2, top = T.testC([[testudata -1 xuxu
                        testudata 2 xuxu
                    gettop
                    return 3]], x)
  assert(res1 and res2 and top == 4)
  
  -- wrong metatable
  res1, res2, top = T.testC([[testudata -1 xuxu1
                  testudata 2 xuxu1
                  gettop
                  return 3]], x)
  assert(not res1 and not res2 and top == 4)
  
  -- non-existent type
  res1, res2, top = T.testC([[testudata -1 xuxu2
                  testudata 2 xuxu2
                  gettop
                  return 3]], x)
  assert(not res1 and not res2 and top == 4)
  
  -- userdata has no metatable
  res1, res2, top = T.testC([[testudata -1 xuxu
                  testudata 2 xuxu
                  gettop
                  return 3]], y)
  assert(not res1 and not res2 and top == 4)
  
  -- erase metatables
  do
    local r = debug.getregistry()
    assert(r.xuxu == mt_xuxu and r.xuxu1 == d)
    r.xuxu = nil; r.xuxu1 = nil
  end
  
  print'OK'
  
  -- $Id: testes/attrib.lua $
-- See Copyright Notice in file all.lua

print "testing require"

assert(require"string" == string)
assert(require"math" == math)
assert(require"table" == table)
assert(require"io" == io)
assert(require"os" == os)
assert(require"coroutine" == coroutine)

assert(type(package.path) == "string")
assert(type(package.cpath) == "string")
assert(type(package.loaded) == "table")
assert(type(package.preload) == "table")

assert(type(package.config) == "string")
print("package config: "..string.gsub(package.config, "\n", "|"))

do
  -- create a path with 'max' templates,
  -- each with 1-10 repetitions of '?'
  local max = _soft and 100 or 2000
  local t = {}
  for i = 1,max do t[i] = string.rep("?", i%10 + 1) end
  t[#t + 1] = ";"    -- empty template
  local path = table.concat(t, ";")
  -- use that path in a search
  local s, err = package.searchpath("xuxu", path)
  -- search fails; check that message has an occurrence of
  -- '??????????' with ? replaced by xuxu and at least 'max' lines
  assert(not s and
         string.find(err, string.rep("xuxu", 10)) and
         #string.gsub(err, "[^\n]", "") >= max)
  -- path with one very long template
  local path = string.rep("?", max)
  local s, err = package.searchpath("xuxu", path)
  assert(not s and string.find(err, string.rep('xuxu', max)))
end

do
  local oldpath = package.path
  package.path = {}
  local s, err = pcall(require, "no-such-file")
  assert(not s and string.find(err, "package.path"))
  package.path = oldpath
end


do  print"testing 'require' message"
  local oldpath = package.path
  local oldcpath = package.cpath

  package.path = "?.lua;?/?"
  package.cpath = "?.so;?/init"

  local st, msg = pcall(require, 'XXX')

  local expected = [[module 'XXX' not found:
	no field package.preload['XXX']
	no file 'XXX.lua'
	no file 'XXX/XXX'
	no file 'XXX.so'
	no file 'XXX/init']]

  assert(msg == expected)

  package.path = oldpath
  package.cpath = oldcpath
end

print('+')


-- The next tests for 'require' assume some specific directories and
-- libraries.

if not _port then --[

local dirsep = string.match(package.config, "^([^\n]+)\n")

-- auxiliary directory with C modules and temporary files
local DIR = "libs" .. dirsep

-- prepend DIR to a name and correct directory separators
local function D (x)
  x = string.gsub(x, "/", dirsep)
  return DIR .. x
end

-- prepend DIR and pospend proper C lib. extension to a name
local function DC (x)
  local ext = (dirsep == '\\') and ".dll" or ".so"
  return D(x .. ext)
end


local function createfiles (files, preextras, posextras)
  for n,c in pairs(files) do
    io.output(D(n))
    io.write(string.format(preextras, n))
    io.write(c)
    io.write(string.format(posextras, n))
    io.close(io.output())
  end
end

function removefiles (files)
  for n in pairs(files) do
    os.remove(D(n))
  end
end

local files = {
  ["names.lua"] = "do return {...} end\n",
  ["err.lua"] = "B = 15; a = a + 1;",
  ["synerr.lua"] = "B =",
  ["A.lua"] = "",
  ["B.lua"] = "assert(...=='B');require 'A'",
  ["A.lc"] = "",
  ["A"] = "",
  ["L"] = "",
  ["XXxX"] = "",
  ["C.lua"] = "package.loaded[...] = 25; require'C'",
}

AA = nil
local extras = [[
NAME = '%s'
REQUIRED = ...
return AA]]

createfiles(files, "", extras)

-- testing explicit "dir" separator in 'searchpath'
assert(package.searchpath("C.lua", D"?", "", "") == D"C.lua")
assert(package.searchpath("C.lua", D"?", ".", ".") == D"C.lua")
assert(package.searchpath("--x-", D"?", "-", "X") == D"XXxX")
assert(package.searchpath("---xX", D"?", "---", "XX") == D"XXxX")
assert(package.searchpath(D"C.lua", "?", dirsep) == D"C.lua")
assert(package.searchpath(".\\C.lua", D"?", "\\") == D"./C.lua")

local oldpath = package.path

package.path = string.gsub("D/?.lua;D/?.lc;D/?;D/??x?;D/L", "D/", DIR)

local try = function (p, n, r, ext)
  NAME = nil
  local rr, x = require(p)
  assert(NAME == n)
  assert(REQUIRED == p)
  assert(rr == r)
  assert(ext == x)
end

a = require"names"
assert(a[1] == "names" and a[2] == D"names.lua")

_G.a = nil
local st, msg = pcall(require, "err")
assert(not st and string.find(msg, "arithmetic") and B == 15)
st, msg = pcall(require, "synerr")
assert(not st and string.find(msg, "error loading module"))

assert(package.searchpath("C", package.path) == D"C.lua")
assert(require"C" == 25)
assert(require"C" == 25)
AA = nil
try('B', 'B.lua', true, "libs/B.lua")
assert(package.loaded.B)
assert(require"B" == true)
assert(package.loaded.A)
assert(require"C" == 25)
package.loaded.A = nil
try('B', nil, true, nil)   -- should not reload package
try('A', 'A.lua', true, "libs/A.lua")
package.loaded.A = nil
os.remove(D'A.lua')
AA = {}
try('A', 'A.lc', AA, "libs/A.lc")  -- now must find second option
assert(package.searchpath("A", package.path) == D"A.lc")
assert(require("A") == AA)
AA = false
try('K', 'L', false, "libs/L")     -- default option
try('K', 'L', false, "libs/L")     -- default option (should reload it)
assert(rawget(_G, "_REQUIREDNAME") == nil)

AA = "x"
try("X", "XXxX", AA, "libs/XXxX")


removefiles(files)


-- testing require of sub-packages

local _G = _G

package.path = string.gsub("D/?.lua;D/?/init.lua", "D/", DIR)

files = {
  ["P1/init.lua"] = "AA = 10",
  ["P1/xuxu.lua"] = "AA = 20",
}

createfiles(files, "_ENV = {}\n", "\nreturn _ENV\n")
AA = 0

local m, ext = assert(require"P1")
assert(ext == "libs/P1/init.lua")
assert(AA == 0 and m.AA == 10)
assert(require"P1" == m)
assert(require"P1" == m)

assert(package.searchpath("P1.xuxu", package.path) == D"P1/xuxu.lua")
m.xuxu, ext = assert(require"P1.xuxu")
assert(AA == 0 and m.xuxu.AA == 20)
assert(ext == "libs/P1/xuxu.lua")
assert(require"P1.xuxu" == m.xuxu)
assert(require"P1.xuxu" == m.xuxu)
assert(require"P1" == m and m.AA == 10)


removefiles(files)


package.path = ""
assert(not pcall(require, "file_does_not_exist"))
package.path = "??\0?"
assert(not pcall(require, "file_does_not_exist1"))

package.path = oldpath

-- check 'require' error message
local fname = "file_does_not_exist2"
local m, err = pcall(require, fname)
for t in string.gmatch(package.path..";"..package.cpath, "[^;]+") do
  t = string.gsub(t, "?", fname)
  assert(string.find(err, t, 1, true))
end

do  -- testing 'package.searchers' not being a table
  local searchers = package.searchers
  package.searchers = 3
  local st, msg = pcall(require, 'a')
  assert(not st and string.find(msg, "must be a table"))
  package.searchers = searchers
end

local function import(...)
  local f = {...}
  return function (m)
    for i=1, #f do m[f[i]] = _G[f[i]] end
  end
end

-- cannot change environment of a C function
assert(not pcall(module, 'XUXU'))



-- testing require of C libraries


local p = ""   -- On Mac OS X, redefine this to "_"

-- check whether loadlib works in this system
local st, err, when = package.loadlib(DC"lib1", "*")
if not st then
  local f, err, when = package.loadlib("donotexist", p.."xuxu")
  assert(not f and type(err) == "string" and when == "absent")
  ;(Message or print)('\n >>> cannot load dynamic library <<<\n')
  print(err, when)
else
  -- tests for loadlib
  local f = assert(package.loadlib(DC"lib1", p.."onefunction"))
  local a, b = f(15, 25)
  assert(a == 25 and b == 15)

  f = assert(package.loadlib(DC"lib1", p.."anotherfunc"))
  assert(f(10, 20) == "10%20\n")

  -- check error messages
  local f, err, when = package.loadlib(DC"lib1", p.."xuxu")
  assert(not f and type(err) == "string" and when == "init")
  f, err, when = package.loadlib("donotexist", p.."xuxu")
  assert(not f and type(err) == "string" and when == "open")

  -- symbols from 'lib1' must be visible to other libraries
  f = assert(package.loadlib(DC"lib11", p.."luaopen_lib11"))
  assert(f() == "exported")

  -- test C modules with prefixes in names
  package.cpath = DC"?"
  local lib2, ext = require"lib2-v2"
  assert(string.find(ext, "libs/lib2-v2", 1, true))
  -- check correct access to global environment and correct
  -- parameters
  assert(_ENV.x == "lib2-v2" and _ENV.y == DC"lib2-v2")
  assert(lib2.id("x") == true)   -- a different "id" implementation

  -- test C submodules
  local fs, ext = require"lib1.sub"
  assert(_ENV.x == "lib1.sub" and _ENV.y == DC"lib1")
  assert(string.find(ext, "libs/lib1", 1, true))
  assert(fs.id(45) == 45)
end

_ENV = _G


-- testing preload

do
  local p = package
  package = {}
  p.preload.pl = function (...)
    local _ENV = {...}
    function xuxu (x) return x+20 end
    return _ENV
  end

  local pl, ext = require"pl"
  assert(require"pl" == pl)
  assert(pl.xuxu(10) == 30)
  assert(pl[1] == "pl" and pl[2] == ":preload:" and ext == ":preload:")

  package = p
  assert(type(package.path) == "string")
end

print('+')

end  --]

print("testing assignments, logical operators, and constructors")

local res, res2 = 27

a, b = 1, 2+3
assert(a==1 and b==5)
a={}
function f() return 10, 11, 12 end
a.x, b, a[1] = 1, 2, f()
assert(a.x==1 and b==2 and a[1]==10)
a[f()], b, a[f()+3] = f(), a, 'x'
assert(a[10] == 10 and b == a and a[13] == 'x')

do
  local f = function (n) local x = {}; for i=1,n do x[i]=i end;
                         return table.unpack(x) end;
  local a,b,c
  a,b = 0, f(1)
  assert(a == 0 and b == 1)
  A,b = 0, f(1)
  assert(A == 0 and b == 1)
  a,b,c = 0,5,f(4)
  assert(a==0 and b==5 and c==1)
  a,b,c = 0,5,f(0)
  assert(a==0 and b==5 and c==nil)
end

a, b, c, d = 1 and nil, 1 or nil, (1 and (nil or 1)), 6
assert(not a and b and c and d==6)

d = 20
a, b, c, d = f()
assert(a==10 and b==11 and c==12 and d==nil)
a,b = f(), 1, 2, 3, f()
assert(a==10 and b==1)

assert(a<b == false and a>b == true)
assert((10 and 2) == 2)
assert((10 or 2) == 10)
assert((10 or assert(nil)) == 10)
assert(not (nil and assert(nil)))
assert((nil or "alo") == "alo")
assert((nil and 10) == nil)
assert((false and 10) == false)
assert((true or 10) == true)
assert((false or 10) == 10)
assert(false ~= nil)
assert(nil ~= false)
assert(not nil == true)
assert(not not nil == false)
assert(not not 1 == true)
assert(not not a == true)
assert(not not (6 or nil) == true)
assert(not not (nil and 56) == false)
assert(not not (nil and true) == false)
assert(not 10 == false)
assert(not {} == false)
assert(not 0.5 == false)
assert(not "x" == false)

assert({} ~= {})
print('+')

a = {}
a[true] = 20
a[false] = 10
assert(a[1<2] == 20 and a[1>2] == 10)

function f(a) return a end

local a = {}
for i=3000,-3000,-1 do a[i + 0.0] = i; end
a[10e30] = "alo"; a[true] = 10; a[false] = 20
assert(a[10e30] == 'alo' and a[not 1] == 20 and a[10<20] == 10)
for i=3000,-3000,-1 do assert(a[i] == i); end
a[print] = assert
a[f] = print
a[a] = a
assert(a[a][a][a][a][print] == assert)
a[print](a[a[f]] == a[print])
assert(not pcall(function () local a = {}; a[nil] = 10 end))
assert(not pcall(function () local a = {[nil] = 10} end))
assert(a[nil] == undef)
a = nil

a = {10,9,8,7,6,5,4,3,2; [-3]='a', [f]=print, a='a', b='ab'}
a, a.x, a.y = a, a[-3]
assert(a[1]==10 and a[-3]==a.a and a[f]==print and a.x=='a' and not a.y)
a[1], f(a)[2], b, c = {['alo']=assert}, 10, a[1], a[f], 6, 10, 23, f(a), 2
a[1].alo(a[2]==10 and b==10 and c==print)

a.aVeryLongName012345678901234567890123456789012345678901234567890123456789 = 10
local function foo ()
  return a.aVeryLongName012345678901234567890123456789012345678901234567890123456789
end
assert(foo() == 10 and
a.aVeryLongName012345678901234567890123456789012345678901234567890123456789 ==
10)



-- test of large float/integer indices 

-- compute maximum integer where all bits fit in a float
local maxint = math.maxinteger

-- trim (if needed) to fit in a float
while maxint ~= (maxint + 0.0) or (maxint - 1) ~= (maxint - 1.0) do
  maxint = maxint // 2
end

maxintF = maxint + 0.0   -- float version

assert(maxintF == maxint and math.type(maxintF) == "float" and
       maxintF >= 2.0^14)

-- floats and integers must index the same places
a[maxintF] = 10; a[maxintF - 1.0] = 11;
a[-maxintF] = 12; a[-maxintF + 1.0] = 13;

assert(a[maxint] == 10 and a[maxint - 1] == 11 and
       a[-maxint] == 12 and a[-maxint + 1] == 13)

a[maxint] = 20
a[-maxint] = 22

assert(a[maxintF] == 20 and a[maxintF - 1.0] == 11 and
       a[-maxintF] == 22 and a[-maxintF + 1.0] == 13)

a = nil


-- test conflicts in multiple assignment
do
  local a,i,j,b
  a = {'a', 'b'}; i=1; j=2; b=a
  i, a[i], a, j, a[j], a[i+j] = j, i, i, b, j, i
  assert(i == 2 and b[1] == 1 and a == 1 and j == b and b[2] == 2 and
         b[3] == 1)
  a = {}
  local function foo ()    -- assigining to upvalues
    b, a.x, a = a, 10, 20
  end
  foo()
  assert(a == 20 and b.x == 10)
end

-- repeat test with upvalues
do
  local a,i,j,b
  a = {'a', 'b'}; i=1; j=2; b=a
  local function foo ()
    i, a[i], a, j, a[j], a[i+j] = j, i, i, b, j, i
  end
  foo()
  assert(i == 2 and b[1] == 1 and a == 1 and j == b and b[2] == 2 and
         b[3] == 1)
  local t = {}
  (function (a) t[a], a = 10, 20  end)(1);
  assert(t[1] == 10)
end

-- bug in 5.2 beta
local function foo ()
  local a
  return function ()
    local b
    a, b = 3, 14    -- local and upvalue have same index
    return a, b
  end
end

local a, b = foo()()
assert(a == 3 and b == 14)

print('OK')

return res

-- $Id: testes/big.lua $
-- See Copyright Notice in file all.lua

if _soft then
    return 'a'
  end
  
  print "testing large tables"
  
  local debug = require"debug" 
  
  local lim = 2^18 + 1000
  local prog = { "local y = {0" }
  for i = 1, lim do prog[#prog + 1] = i  end
  prog[#prog + 1] = "}\n"
  prog[#prog + 1] = "X = y\n"
  prog[#prog + 1] = ("assert(X[%d] == %d)"):format(lim - 1, lim - 2)
  prog[#prog + 1] = "return 0"
  prog = table.concat(prog, ";")
  
  local env = {string = string, assert = assert}
  local f = assert(load(prog, nil, nil, env))
  
  f()
  assert(env.X[lim] == lim - 1 and env.X[lim + 1] == lim)
  for k in pairs(env) do env[k] = undef end
  
  -- yields during accesses larger than K (in RK)
  setmetatable(env, {
    __index = function (t, n) coroutine.yield('g'); return _G[n] end,
    __newindex = function (t, n, v) coroutine.yield('s'); _G[n] = v end,
  })
  
  X = nil
  co = coroutine.wrap(f)
  assert(co() == 's')
  assert(co() == 'g')
  assert(co() == 'g')
  assert(co() == 0)
  
  assert(X[lim] == lim - 1 and X[lim + 1] == lim)
  
  -- errors in accesses larger than K (in RK)
  getmetatable(env).__index = function () end
  getmetatable(env).__newindex = function () end
  local e, m = pcall(f)
  assert(not e and m:find("global 'X'"))
  
  -- errors in metamethods 
  getmetatable(env).__newindex = function () error("hi") end
  local e, m = xpcall(f, debug.traceback)
  assert(not e and m:find("'newindex'"))
  
  f, X = nil
  
  coroutine.yield'b'
  
  if 2^32 == 0 then   -- (small integers) {   
  
  print "testing string length overflow"
  
  local repstrings = 192          -- number of strings to be concatenated
  local ssize = math.ceil(2.0^32 / repstrings) + 1   -- size of each string
  
  assert(repstrings * ssize > 2.0^32)  -- it should be larger than maximum size
  
  local longs = string.rep("\0", ssize)   -- create one long string
  
  -- create function to concatenate 'repstrings' copies of its argument
  local rep = assert(load(
    "local a = ...; return " .. string.rep("a", repstrings, "..")))
  
  local a, b = pcall(rep, longs)   -- call that function
  
  -- it should fail without creating string (result would be too large)
  assert(not a and string.find(b, "overflow"))
  
  end   -- }
  
  print'OK'
  
  return 'a'
  -- $Id: testes/bitwise.lua $
-- See Copyright Notice in file all.lua

print("testing bitwise operations")

require "bwcoercion"

local numbits = string.packsize('j') * 8

assert(~0 == -1)

assert((1 << (numbits - 1)) == math.mininteger)

-- basic tests for bitwise operators;
-- use variables to avoid constant folding
local a, b, c, d
a = 0xFFFFFFFFFFFFFFFF
assert(a == -1 and a & -1 == a and a & 35 == 35)
a = 0xF0F0F0F0F0F0F0F0
assert(a | -1 == -1)
assert(a ~ a == 0 and a ~ 0 == a and a ~ ~a == -1)
assert(a >> 4 == ~a)
a = 0xF0; b = 0xCC; c = 0xAA; d = 0xFD
assert(a | b ~ c & d == 0xF4)

a = 0xF0.0; b = 0xCC.0; c = "0xAA.0"; d = "0xFD.0"
assert(a | b ~ c & d == 0xF4)

a = 0xF0000000; b = 0xCC000000;
c = 0xAA000000; d = 0xFD000000
assert(a | b ~ c & d == 0xF4000000)
assert(~~a == a and ~a == -1 ~ a and -d == ~d + 1)

a = a << 32
b = b << 32
c = c << 32
d = d << 32
assert(a | b ~ c & d == 0xF4000000 << 32)
assert(~~a == a and ~a == -1 ~ a and -d == ~d + 1)

assert(-1 >> 1 == (1 << (numbits - 1)) - 1 and 1 << 31 == 0x80000000)
assert(-1 >> (numbits - 1) == 1)
assert(-1 >> numbits == 0 and
       -1 >> -numbits == 0 and
       -1 << numbits == 0 and
       -1 << -numbits == 0)

assert(1 >> math.mininteger == 0)
assert(1 >> math.maxinteger == 0)
assert(1 << math.mininteger == 0)
assert(1 << math.maxinteger == 0)

assert((2^30 - 1) << 2^30 == 0)
assert((2^30 - 1) >> 2^30 == 0)

assert(1 >> -3 == 1 << 3 and 1000 >> 5 == 1000 << -5)


-- coercion from strings to integers
assert("0xffffffffffffffff" | 0 == -1)
assert("0xfffffffffffffffe" & "-1" == -2)
assert(" \t-0xfffffffffffffffe\n\t" & "-1" == 2)
assert("   \n  -45  \t " >> "  -2  " == -45 * 4)
assert("1234.0" << "5.0" == 1234 * 32)
assert("0xffff.0" ~ "0xAAAA" == 0x5555)
assert(~"0x0.000p4" == -1)

assert(("7" .. 3) << 1 == 146)
assert(0xffffffff >> (1 .. "9") == 0x1fff)
assert(10 | (1 .. "9") == 27)

do
  local st, msg = pcall(function () return 4 & "a" end)
  assert(string.find(msg, "'band'"))

  local st, msg = pcall(function () return ~"a" end)
  assert(string.find(msg, "'bnot'"))
end


-- out of range number
assert(not pcall(function () return "0xffffffffffffffff.0" | 0 end))

-- embedded zeros
assert(not pcall(function () return "0xffffffffffffffff\0" | 0 end))

print'+'


package.preload.bit32 = function ()     --{

-- no built-in 'bit32' library: implement it using bitwise operators

local bit = {}

function bit.bnot (a)
  return ~a & 0xFFFFFFFF
end


--
-- in all vararg functions, avoid creating 'arg' table when there are
-- only 2 (or less) parameters, as 2 parameters is the common case
--

function bit.band (x, y, z, ...)
  if not z then
    return ((x or -1) & (y or -1)) & 0xFFFFFFFF
  else
    local arg = {...}
    local res = x & y & z
    for i = 1, #arg do res = res & arg[i] end
    return res & 0xFFFFFFFF
  end
end

function bit.bor (x, y, z, ...)
  if not z then
    return ((x or 0) | (y or 0)) & 0xFFFFFFFF
  else
    local arg = {...}
    local res = x | y | z
    for i = 1, #arg do res = res | arg[i] end
    return res & 0xFFFFFFFF
  end
end

function bit.bxor (x, y, z, ...)
  if not z then
    return ((x or 0) ~ (y or 0)) & 0xFFFFFFFF
  else
    local arg = {...}
    local res = x ~ y ~ z
    for i = 1, #arg do res = res ~ arg[i] end
    return res & 0xFFFFFFFF
  end
end

function bit.btest (...)
  return bit.band(...) ~= 0
end

function bit.lshift (a, b)
  return ((a & 0xFFFFFFFF) << b) & 0xFFFFFFFF
end

function bit.rshift (a, b)
  return ((a & 0xFFFFFFFF) >> b) & 0xFFFFFFFF
end

function bit.arshift (a, b)
  a = a & 0xFFFFFFFF
  if b <= 0 or (a & 0x80000000) == 0 then
    return (a >> b) & 0xFFFFFFFF
  else
    return ((a >> b) | ~(0xFFFFFFFF >> b)) & 0xFFFFFFFF
  end
end

function bit.lrotate (a ,b)
  b = b & 31
  a = a & 0xFFFFFFFF
  a = (a << b) | (a >> (32 - b))
  return a & 0xFFFFFFFF
end

function bit.rrotate (a, b)
  return bit.lrotate(a, -b)
end

local function checkfield (f, w)
  w = w or 1
  assert(f >= 0, "field cannot be negative")
  assert(w > 0, "width must be positive")
  assert(f + w <= 32, "trying to access non-existent bits")
  return f, ~(-1 << w)
end

function bit.extract (a, f, w)
  local f, mask = checkfield(f, w)
  return (a >> f) & mask
end

function bit.replace (a, v, f, w)
  local f, mask = checkfield(f, w)
  v = v & mask
  a = (a & ~(mask << f)) | (v << f)
  return a & 0xFFFFFFFF
end

return bit

end  --}


print("testing bitwise library")

local bit32 = require'bit32'

assert(bit32.band() == bit32.bnot(0))
assert(bit32.btest() == true)
assert(bit32.bor() == 0)
assert(bit32.bxor() == 0)

assert(bit32.band() == bit32.band(0xffffffff))
assert(bit32.band(1,2) == 0)


-- out-of-range numbers
assert(bit32.band(-1) == 0xffffffff)
assert(bit32.band((1 << 33) - 1) == 0xffffffff)
assert(bit32.band(-(1 << 33) - 1) == 0xffffffff)
assert(bit32.band((1 << 33) + 1) == 1)
assert(bit32.band(-(1 << 33) + 1) == 1)
assert(bit32.band(-(1 << 40)) == 0)
assert(bit32.band(1 << 40) == 0)
assert(bit32.band(-(1 << 40) - 2) == 0xfffffffe)
assert(bit32.band((1 << 40) - 4) == 0xfffffffc)

assert(bit32.lrotate(0, -1) == 0)
assert(bit32.lrotate(0, 7) == 0)
assert(bit32.lrotate(0x12345678, 0) == 0x12345678)
assert(bit32.lrotate(0x12345678, 32) == 0x12345678)
assert(bit32.lrotate(0x12345678, 4) == 0x23456781)
assert(bit32.rrotate(0x12345678, -4) == 0x23456781)
assert(bit32.lrotate(0x12345678, -8) == 0x78123456)
assert(bit32.rrotate(0x12345678, 8) == 0x78123456)
assert(bit32.lrotate(0xaaaaaaaa, 2) == 0xaaaaaaaa)
assert(bit32.lrotate(0xaaaaaaaa, -2) == 0xaaaaaaaa)
for i = -50, 50 do
  assert(bit32.lrotate(0x89abcdef, i) == bit32.lrotate(0x89abcdef, i%32))
end

assert(bit32.lshift(0x12345678, 4) == 0x23456780)
assert(bit32.lshift(0x12345678, 8) == 0x34567800)
assert(bit32.lshift(0x12345678, -4) == 0x01234567)
assert(bit32.lshift(0x12345678, -8) == 0x00123456)
assert(bit32.lshift(0x12345678, 32) == 0)
assert(bit32.lshift(0x12345678, -32) == 0)
assert(bit32.rshift(0x12345678, 4) == 0x01234567)
assert(bit32.rshift(0x12345678, 8) == 0x00123456)
assert(bit32.rshift(0x12345678, 32) == 0)
assert(bit32.rshift(0x12345678, -32) == 0)
assert(bit32.arshift(0x12345678, 0) == 0x12345678)
assert(bit32.arshift(0x12345678, 1) == 0x12345678 // 2)
assert(bit32.arshift(0x12345678, -1) == 0x12345678 * 2)
assert(bit32.arshift(-1, 1) == 0xffffffff)
assert(bit32.arshift(-1, 24) == 0xffffffff)
assert(bit32.arshift(-1, 32) == 0xffffffff)
assert(bit32.arshift(-1, -1) == bit32.band(-1 * 2, 0xffffffff))

assert(0x12345678 << 4 == 0x123456780)
assert(0x12345678 << 8 == 0x1234567800)
assert(0x12345678 << -4 == 0x01234567)
assert(0x12345678 << -8 == 0x00123456)
assert(0x12345678 << 32 == 0x1234567800000000)
assert(0x12345678 << -32 == 0)
assert(0x12345678 >> 4 == 0x01234567)
assert(0x12345678 >> 8 == 0x00123456)
assert(0x12345678 >> 32 == 0)
assert(0x12345678 >> -32 == 0x1234567800000000)

print("+")
-- some special cases
local c = {0, 1, 2, 3, 10, 0x80000000, 0xaaaaaaaa, 0x55555555,
           0xffffffff, 0x7fffffff}

for _, b in pairs(c) do
  assert(bit32.band(b) == b)
  assert(bit32.band(b, b) == b)
  assert(bit32.band(b, b, b, b) == b)
  assert(bit32.btest(b, b) == (b ~= 0))
  assert(bit32.band(b, b, b) == b)
  assert(bit32.band(b, b, b, ~b) == 0)
  assert(bit32.btest(b, b, b) == (b ~= 0))
  assert(bit32.band(b, bit32.bnot(b)) == 0)
  assert(bit32.bor(b, bit32.bnot(b)) == bit32.bnot(0))
  assert(bit32.bor(b) == b)
  assert(bit32.bor(b, b) == b)
  assert(bit32.bor(b, b, b) == b)
  assert(bit32.bor(b, b, 0, ~b) == 0xffffffff)
  assert(bit32.bxor(b) == b)
  assert(bit32.bxor(b, b) == 0)
  assert(bit32.bxor(b, b, b) == b)
  assert(bit32.bxor(b, b, b, b) == 0)
  assert(bit32.bxor(b, 0) == b)
  assert(bit32.bnot(b) ~= b)
  assert(bit32.bnot(bit32.bnot(b)) == b)
  assert(bit32.bnot(b) == (1 << 32) - 1 - b)
  assert(bit32.lrotate(b, 32) == b)
  assert(bit32.rrotate(b, 32) == b)
  assert(bit32.lshift(bit32.lshift(b, -4), 4) == bit32.band(b, bit32.bnot(0xf)))
  assert(bit32.rshift(bit32.rshift(b, 4), -4) == bit32.band(b, bit32.bnot(0xf)))
end

-- for this test, use at most 24 bits (mantissa of a single float)
c = {0, 1, 2, 3, 10, 0x800000, 0xaaaaaa, 0x555555, 0xffffff, 0x7fffff}
for _, b in pairs(c) do
  for i = -40, 40 do
    local x = bit32.lshift(b, i)
    local y = math.floor(math.fmod(b * 2.0^i, 2.0^32))
    assert(math.fmod(x - y, 2.0^32) == 0)
  end
end

assert(not pcall(bit32.band, {}))
assert(not pcall(bit32.bnot, "a"))
assert(not pcall(bit32.lshift, 45))
assert(not pcall(bit32.lshift, 45, print))
assert(not pcall(bit32.rshift, 45, print))

print("+")


-- testing extract/replace

assert(bit32.extract(0x12345678, 0, 4) == 8)
assert(bit32.extract(0x12345678, 4, 4) == 7)
assert(bit32.extract(0xa0001111, 28, 4) == 0xa)
assert(bit32.extract(0xa0001111, 31, 1) == 1)
assert(bit32.extract(0x50000111, 31, 1) == 0)
assert(bit32.extract(0xf2345679, 0, 32) == 0xf2345679)

assert(not pcall(bit32.extract, 0, -1))
assert(not pcall(bit32.extract, 0, 32))
assert(not pcall(bit32.extract, 0, 0, 33))
assert(not pcall(bit32.extract, 0, 31, 2))

assert(bit32.replace(0x12345678, 5, 28, 4) == 0x52345678)
assert(bit32.replace(0x12345678, 0x87654321, 0, 32) == 0x87654321)
assert(bit32.replace(0, 1, 2) == 2^2)
assert(bit32.replace(0, -1, 4) == 2^4)
assert(bit32.replace(-1, 0, 31) == (1 << 31) - 1)
assert(bit32.replace(-1, 0, 1, 2) == (1 << 32) - 7)


-- testing conversion of floats

assert(bit32.bor(3.0) == 3)
assert(bit32.bor(-4.0) == 0xfffffffc)

-- large floats and large-enough integers?
if 2.0^50 < 2.0^50 + 1.0 and 2.0^50 < (-1 >> 1) then
  assert(bit32.bor(2.0^32 - 5.0) == 0xfffffffb)
  assert(bit32.bor(-2.0^32 - 6.0) == 0xfffffffa)
  assert(bit32.bor(2.0^48 - 5.0) == 0xfffffffb)
  assert(bit32.bor(-2.0^48 - 6.0) == 0xfffffffa)
end

print'OK'

local tonumber, tointeger = tonumber, math.tointeger
local type, getmetatable, rawget, error = type, getmetatable, rawget, error
local strsub = string.sub

local print = print

_ENV = nil

-- Try to convert a value to an integer, without assuming any coercion.
local function toint (x)
  x = tonumber(x)   -- handle numerical strings
  if not x then
    return false    -- not coercible to a number
  end
  return tointeger(x)
end


-- If operation fails, maybe second operand has a metamethod that should
-- have been called if not for this string metamethod, so try to
-- call it.
local function trymt (x, y, mtname)
  if type(y) ~= "string" then    -- avoid recalling original metamethod
    local mt = getmetatable(y)
    local mm = mt and rawget(mt, mtname)
    if mm then
      return mm(x, y)
    end
  end
  -- if any test fails, there is no other metamethod to be called
  error("attempt to '" .. strsub(mtname, 3) ..
        "' a " .. type(x) .. " with a " .. type(y), 4)
end


local function checkargs (x, y, mtname)
  local xi = toint(x)
  local yi = toint(y)
  if xi and yi then
    return xi, yi
  else
    return trymt(x, y, mtname), nil
  end
end


local smt = getmetatable("")

smt.__band = function (x, y)
  local x, y = checkargs(x, y, "__band")
  return y and x & y or x
end

smt.__bor = function (x, y)
  local x, y = checkargs(x, y, "__bor")
  return y and x | y or x
end

smt.__bxor = function (x, y)
  local x, y = checkargs(x, y, "__bxor")
  return y and x ~ y or x
end

smt.__shl = function (x, y)
  local x, y = checkargs(x, y, "__shl")
  return y and x << y or x
end

smt.__shr = function (x, y)
  local x, y = checkargs(x, y, "__shr")
  return y and x >> y or x
end

smt.__bnot = function (x)
  local x, y = checkargs(x, x, "__bnot")
  return y and ~x or x
end

-- $Id: testes/calls.lua $
-- See Copyright Notice in file all.lua

print("testing functions and calls")

local debug = require "debug"

-- get the opportunity to test 'type' too ;)

assert(type(1<2) == 'boolean')
assert(type(true) == 'boolean' and type(false) == 'boolean')
assert(type(nil) == 'nil'
   and type(-3) == 'number'
   and type'x' == 'string'
   and type{} == 'table'
   and type(type) == 'function')

assert(type(assert) == type(print))
function f (x) return a:x (x) end
assert(type(f) == 'function')
assert(not pcall(type))


-- testing local-function recursion
fact = false
do
  local res = 1
  local function fact (n)
    if n==0 then return res
    else return n*fact(n-1)
    end
  end
  assert(fact(5) == 120)
end
assert(fact == false)

-- testing declarations
a = {i = 10}
self = 20
function a:x (x) return x+self.i end
function a.y (x) return x+self end

assert(a:x(1)+10 == a.y(1))

a.t = {i=-100}
a["t"].x = function (self, a,b) return self.i+a+b end

assert(a.t:x(2,3) == -95)

do
  local a = {x=0}
  function a:add (x) self.x, a.y = self.x+x, 20; return self end
  assert(a:add(10):add(20):add(30).x == 60 and a.y == 20)
end

local a = {b={c={}}}

function a.b.c.f1 (x) return x+1 end
function a.b.c:f2 (x,y) self[x] = y end
assert(a.b.c.f1(4) == 5)
a.b.c:f2('k', 12); assert(a.b.c.k == 12)

print('+')

t = nil   -- 'declare' t
function f(a,b,c) local d = 'a'; t={a,b,c,d} end

f(      -- this line change must be valid
  1,2)
assert(t[1] == 1 and t[2] == 2 and t[3] == nil and t[4] == 'a')
f(1,2,   -- this one too
      3,4)
assert(t[1] == 1 and t[2] == 2 and t[3] == 3 and t[4] == 'a')

function fat(x)
  if x <= 1 then return 1
  else return x*load("return fat(" .. x-1 .. ")", "")()
  end
end

assert(load "load 'assert(fat(6)==720)' () ")()
a = load('return fat(5), 3')
a,b = a()
assert(a == 120 and b == 3)
print('+')

function err_on_n (n)
  if n==0 then error(); exit(1);
  else err_on_n (n-1); exit(1);
  end
end

do
  function dummy (n)
    if n > 0 then
      assert(not pcall(err_on_n, n))
      dummy(n-1)
    end
  end
end

dummy(10)

function deep (n)
  if n>0 then deep(n-1) end
end
deep(10)
deep(180)


print"testing tail calls"

function deep (n) if n>0 then return deep(n-1) else return 101 end end
assert(deep(30000) == 101)
a = {}
function a:deep (n) if n>0 then return self:deep(n-1) else return 101 end end
assert(a:deep(30000) == 101)

do   -- tail calls x varargs
  local function foo (x, ...) local a = {...}; return x, a[1], a[2] end

  local function foo1 (x) return foo(10, x, x + 1) end

  local a, b, c = foo1(-2)
  assert(a == 10 and b == -2 and c == -1)

  -- tail calls x metamethods
  local t = setmetatable({}, {__call = foo})
  local function foo2 (x) return t(10, x) end
  a, b, c = foo2(100)
  assert(a == t and b == 10 and c == 100)

  a, b = (function () return foo() end)()
  assert(a == nil and b == nil)

  local X, Y, A
  local function foo (x, y, ...) X = x; Y = y; A = {...} end
  local function foo1 (...) return foo(...) end

  local a, b, c = foo1()
  assert(X == nil and Y == nil and #A == 0)

  a, b, c = foo1(10)
  assert(X == 10 and Y == nil and #A == 0)

  a, b, c = foo1(10, 20)
  assert(X == 10 and Y == 20 and #A == 0)

  a, b, c = foo1(10, 20, 30)
  assert(X == 10 and Y == 20 and #A == 1 and A[1] == 30)
end



do   -- tail calls x chain of __call
  local n = 10000   -- depth

  local function foo ()
    if n == 0 then return 1023
    else n = n - 1; return foo()
    end
  end

  -- build a chain of __call metamethods ending in function 'foo'
  for i = 1, 100 do
    foo = setmetatable({}, {__call = foo})
  end

  -- call the first one as a tail call in a new coroutine
  -- (to ensure stack is not preallocated)
  assert(coroutine.wrap(function() return foo() end)() == 1023)
end

print('+')


do  -- testing chains of '__call'
  local N = 20
  local u = table.pack
  for i = 1, N do
    u = setmetatable({i}, {__call = u})
  end

  local Res = u("a", "b", "c")

  assert(Res.n == N + 3)
  for i = 1, N do
    assert(Res[i][1] == i)
  end
  assert(Res[N + 1] == "a" and Res[N + 2] == "b" and Res[N + 3] == "c")
end


a = nil
(function (x) a=x end)(23)
assert(a == 23 and (function (x) return x*2 end)(20) == 40)


-- testing closures

-- fixed-point operator
Z = function (le)
      local function a (f)
        return le(function (x) return f(f)(x) end)
      end
      return a(a)
    end


-- non-recursive factorial

F = function (f)
      return function (n)
               if n == 0 then return 1
               else return n*f(n-1) end
             end
    end

fat = Z(F)

assert(fat(0) == 1 and fat(4) == 24 and Z(F)(5)==5*Z(F)(4))

local function g (z)
  local function f (a,b,c,d)
    return function (x,y) return a+b+c+d+a+x+y+z end
  end
  return f(z,z+1,z+2,z+3)
end

f = g(10)
assert(f(9, 16) == 10+11+12+13+10+9+16+10)

Z, F, f = nil
print('+')

-- testing multiple returns

function unlpack (t, i)
  i = i or 1
  if (i <= #t) then
    return t[i], unlpack(t, i+1)
  end
end

function equaltab (t1, t2)
  assert(#t1 == #t2)
  for i = 1, #t1 do
    assert(t1[i] == t2[i])
  end
end

local pack = function (...) return (table.pack(...)) end

function f() return 1,2,30,4 end
function ret2 (a,b) return a,b end

local a,b,c,d = unlpack{1,2,3}
assert(a==1 and b==2 and c==3 and d==nil)
a = {1,2,3,4,false,10,'alo',false,assert}
equaltab(pack(unlpack(a)), a)
equaltab(pack(unlpack(a), -1), {1,-1})
a,b,c,d = ret2(f()), ret2(f())
assert(a==1 and b==1 and c==2 and d==nil)
a,b,c,d = unlpack(pack(ret2(f()), ret2(f())))
assert(a==1 and b==1 and c==2 and d==nil)
a,b,c,d = unlpack(pack(ret2(f()), (ret2(f()))))
assert(a==1 and b==1 and c==nil and d==nil)

a = ret2{ unlpack{1,2,3}, unlpack{3,2,1}, unlpack{"a", "b"}}
assert(a[1] == 1 and a[2] == 3 and a[3] == "a" and a[4] == "b")


-- testing calls with 'incorrect' arguments
rawget({}, "x", 1)
rawset({}, "x", 1, 2)
assert(math.sin(1,2) == math.sin(1))
table.sort({10,9,8,4,19,23,0,0}, function (a,b) return a<b end, "extra arg")


-- test for generic load
local x = "-- a comment\0\0\0\n  x = 10 + \n23; \
     local a = function () x = 'hi' end; \
     return '\0'"
function read1 (x)
  local i = 0
  return function ()
    collectgarbage()
    i=i+1
    return string.sub(x, i, i)
  end
end

function cannotload (msg, a,b)
  assert(not a and string.find(b, msg))
end

a = assert(load(read1(x), "modname", "t", _G))
assert(a() == "\0" and _G.x == 33)
assert(debug.getinfo(a).source == "modname")
-- cannot read text in binary mode
cannotload("attempt to load a text chunk", load(read1(x), "modname", "b", {}))
cannotload("attempt to load a text chunk", load(x, "modname", "b"))

a = assert(load(function () return nil end))
a()  -- empty chunk

assert(not load(function () return true end))


-- small bug
local t = {nil, "return ", "3"}
f, msg = load(function () return table.remove(t, 1) end)
assert(f() == nil)   -- should read the empty chunk

-- another small bug (in 5.2.1)
f = load(string.dump(function () return 1 end), nil, "b", {})
assert(type(f) == "function" and f() == 1)


do   -- another bug (in 5.4.0)
  -- loading a binary long string interrupted by GC cycles
  local f = string.dump(function ()
    return '01234567890123456789012345678901234567890123456789'
  end)
  f = load(read1(f))
  assert(f() == '01234567890123456789012345678901234567890123456789')
end


x = string.dump(load("x = 1; return x"))
a = assert(load(read1(x), nil, "b"))
assert(a() == 1 and _G.x == 1)
cannotload("attempt to load a binary chunk", load(read1(x), nil, "t"))
cannotload("attempt to load a binary chunk", load(x, nil, "t"))

assert(not pcall(string.dump, print))  -- no dump of C functions

cannotload("unexpected symbol", load(read1("*a = 123")))
cannotload("unexpected symbol", load("*a = 123"))
cannotload("hhi", load(function () error("hhi") end))

-- any value is valid for _ENV
assert(load("return _ENV", nil, nil, 123)() == 123)


-- load when _ENV is not first upvalue
local x; XX = 123
local function h ()
  local y=x   -- use 'x', so that it becomes 1st upvalue
  return XX   -- global name
end
local d = string.dump(h)
x = load(d, "", "b")
assert(debug.getupvalue(x, 2) == '_ENV')
debug.setupvalue(x, 2, _G)
assert(x() == 123)

assert(assert(load("return XX + ...", nil, nil, {XX = 13}))(4) == 17)


-- test generic load with nested functions
x = [[
  return function (x)
    return function (y)
     return function (z)
       return x+y+z
     end
   end
  end
]]
a = assert(load(read1(x), "read", "t"))
assert(a()(2)(3)(10) == 15)

-- repeat the test loading a binary chunk
x = string.dump(a)
a = assert(load(read1(x), "read", "b"))
assert(a()(2)(3)(10) == 15)


-- test for dump/undump with upvalues
local a, b = 20, 30
x = load(string.dump(function (x)
  if x == "set" then a = 10+b; b = b+1 else
  return a
  end
end), "", "b", nil)
assert(x() == nil)
assert(debug.setupvalue(x, 1, "hi") == "a")
assert(x() == "hi")
assert(debug.setupvalue(x, 2, 13) == "b")
assert(not debug.setupvalue(x, 3, 10))   -- only 2 upvalues
x("set")
assert(x() == 23)
x("set")
assert(x() == 24)

-- test for dump/undump with many upvalues
do
  local nup = 200    -- maximum number of local variables
  local prog = {"local a1"}
  for i = 2, nup do prog[#prog + 1] = ", a" .. i end
  prog[#prog + 1] = " = 1"
  for i = 2, nup do prog[#prog + 1] = ", " .. i end
  local sum = 1
  prog[#prog + 1] = "; return function () return a1"
  for i = 2, nup do prog[#prog + 1] = " + a" .. i; sum = sum + i end
  prog[#prog + 1] = " end"
  prog = table.concat(prog)
  local f = assert(load(prog))()
  assert(f() == sum)

  f = load(string.dump(f))   -- main chunk now has many upvalues
  local a = 10
  local h = function () return a end
  for i = 1, nup do
    debug.upvaluejoin(f, i, h, 1)
  end
  assert(f() == 10 * nup)
end

-- test for long method names
do
  local t = {x = 1}
  function t:_012345678901234567890123456789012345678901234567890123456789 ()
    return self.x
  end
  assert(t:_012345678901234567890123456789012345678901234567890123456789() == 1)
end


-- test for bug in parameter adjustment
assert((function () return nil end)(4) == nil)
assert((function () local a; return a end)(4) == nil)
assert((function (a) return a end)() == nil)


print("testing binary chunks")
do
  local header = string.pack("c4BBc6BBB",
    "\27Lua",                                  -- signature
    0x54,                                      -- version 5.4 (0x54)
    0,                                         -- format
    "\x19\x93\r\n\x1a\n",                      -- data
    4,                                         -- size of instruction
    string.packsize("j"),                      -- sizeof(lua integer)
    string.packsize("n")                       -- sizeof(lua number)
  )
  local c = string.dump(function ()
    local a = 1; local b = 3;
    local f = function () return a + b + _ENV.c; end    -- upvalues
    local s1 = "a constant"
    local s2 = "another constant"
    return a + b * 3
  end)

  assert(assert(load(c))() == 10)

  -- check header
  assert(string.sub(c, 1, #header) == header)
  -- check LUAC_INT and LUAC_NUM
  local ci, cn = string.unpack("jn", c, #header + 1)
  assert(ci == 0x5678 and cn == 370.5)

  -- corrupted header
  for i = 1, #header do
    local s = string.sub(c, 1, i - 1) ..
              string.char(string.byte(string.sub(c, i, i)) + 1) ..
              string.sub(c, i + 1, -1)
    assert(#s == #c)
    assert(not load(s))
  end

  -- loading truncated binary chunks
  for i = 1, #c - 1 do
    local st, msg = load(string.sub(c, 1, i))
    assert(not st and string.find(msg, "truncated"))
  end
end

print('OK')
return deep

-- $Id: testes/closure.lua $
-- See Copyright Notice in file all.lua

print "testing closures"

local A,B = 0,{g=10}
function f(x)
  local a = {}
  for i=1,1000 do
    local y = 0
    do
      a[i] = function () B.g = B.g+1; y = y+x; return y+A end
    end
  end
  local dummy = function () return a[A] end
  collectgarbage()
  A = 1; assert(dummy() == a[1]); A = 0;
  assert(a[1]() == x)
  assert(a[3]() == x)
  collectgarbage()
  assert(B.g == 12)
  return a
end

local a = f(10)
-- force a GC in this level
local x = {[1] = {}}   -- to detect a GC
setmetatable(x, {__mode = 'kv'})
while x[1] do   -- repeat until GC
  local a = A..A..A..A  -- create garbage
  A = A+1
end
assert(a[1]() == 20+A)
assert(a[1]() == 30+A)
assert(a[2]() == 10+A)
collectgarbage()
assert(a[2]() == 20+A)
assert(a[2]() == 30+A)
assert(a[3]() == 20+A)
assert(a[8]() == 10+A)
assert(getmetatable(x).__mode == 'kv')
assert(B.g == 19)


-- testing equality
a = {}

for i = 1, 5 do  a[i] = function (x) return i + a + _ENV end  end
assert(a[3] ~= a[4] and a[4] ~= a[5])

do
  local a = function (x)  return math.sin(_ENV[x])  end
  local function f()
    return a
  end
  assert(f() == f())
end


-- testing closures with 'for' control variable
a = {}
for i=1,10 do
  a[i] = {set = function(x) i=x end, get = function () return i end}
  if i == 3 then break end
end
assert(a[4] == undef)
a[1].set(10)
assert(a[2].get() == 2)
a[2].set('a')
assert(a[3].get() == 3)
assert(a[2].get() == 'a')

a = {}
local t = {"a", "b"}
for i = 1, #t do
  local k = t[i]
  a[i] = {set = function(x, y) i=x; k=y end,
          get = function () return i, k end}
  if i == 2 then break end
end
a[1].set(10, 20)
local r,s = a[2].get()
assert(r == 2 and s == 'b')
r,s = a[1].get()
assert(r == 10 and s == 20)
a[2].set('a', 'b')
r,s = a[2].get()
assert(r == "a" and s == "b")


-- testing closures with 'for' control variable x break
for i=1,3 do
  f = function () return i end
  break
end
assert(f() == 1)

for k = 1, #t do
  local v = t[k]
  f = function () return k, v end
  break
end
assert(({f()})[1] == 1)
assert(({f()})[2] == "a")


-- testing closure x break x return x errors

local b
function f(x)
  local first = 1
  while 1 do
    if x == 3 and not first then return end
    local a = 'xuxu'
    b = function (op, y)
          if op == 'set' then
            a = x+y
          else
            return a
          end
        end
    if x == 1 then do break end
    elseif x == 2 then return
    else if x ~= 3 then error() end
    end
    first = nil
  end
end

for i=1,3 do
  f(i)
  assert(b('get') == 'xuxu')
  b('set', 10); assert(b('get') == 10+i)
  b = nil
end

pcall(f, 4);
assert(b('get') == 'xuxu')
b('set', 10); assert(b('get') == 14)


local w
-- testing multi-level closure
function f(x)
  return function (y)
    return function (z) return w+x+y+z end
  end
end

y = f(10)
w = 1.345
assert(y(20)(30) == 60+w)


-- testing closures x break
do
  local X, Y
  local a = math.sin(0)

  while a do
    local b = 10
    X = function () return b end   -- closure with upvalue
    if a then break end
  end
  
  do
    local b = 20
    Y = function () return b end   -- closure with upvalue
  end

  -- upvalues must be different
  assert(X() == 10 and Y() == 20)
end

  
-- testing closures x repeat-until

local a = {}
local i = 1
repeat
  local x = i
  a[i] = function () i = x+1; return x end
until i > 10 or a[i]() ~= x
assert(i == 11 and a[1]() == 1 and a[3]() == 3 and i == 4)


-- testing closures created in 'then' and 'else' parts of 'if's
a = {}
for i = 1, 10 do
  if i % 3 == 0 then
    local y = 0
    a[i] = function (x) local t = y; y = x; return t end
  elseif i % 3 == 1 then
    goto L1
    error'not here'
  ::L1::
    local y = 1
    a[i] = function (x) local t = y; y = x; return t end
  elseif i % 3 == 2 then
    local t
    goto l4
    ::l4a:: a[i] = t; goto l4b
    error("should never be here!")
    ::l4::
    local y = 2
    t = function (x) local t = y; y = x; return t end
    goto l4a
    error("should never be here!")
    ::l4b::
  end
end

for i = 1, 10 do
  assert(a[i](i * 10) == i % 3 and a[i]() == i * 10)
end

print'+'


-- test for correctly closing upvalues in tail calls of vararg functions
local function t ()
  local function c(a,b) assert(a=="test" and b=="OK") end
  local function v(f, ...) c("test", f() ~= 1 and "FAILED" or "OK") end
  local x = 1
  return v(function() return x end)
end
t()


-- test for debug manipulation of upvalues
local debug = require'debug'

do
  local a , b, c = 3, 5, 7
  foo1 = function () return a+b end;
  foo2 = function () return b+a end;
  do
    local a = 10
    foo3 = function () return a+b end;
  end
end

assert(debug.upvalueid(foo1, 1))
assert(debug.upvalueid(foo1, 2))
assert(not debug.upvalueid(foo1, 3))
assert(debug.upvalueid(foo1, 1) == debug.upvalueid(foo2, 2))
assert(debug.upvalueid(foo1, 2) == debug.upvalueid(foo2, 1))
assert(debug.upvalueid(foo3, 1))
assert(debug.upvalueid(foo1, 1) ~= debug.upvalueid(foo3, 1))
assert(debug.upvalueid(foo1, 2) == debug.upvalueid(foo3, 2))

assert(debug.upvalueid(string.gmatch("x", "x"), 1) ~= nil)

assert(foo1() == 3 + 5 and foo2() == 5 + 3)
debug.upvaluejoin(foo1, 2, foo2, 2)
assert(foo1() == 3 + 3 and foo2() == 5 + 3)
assert(foo3() == 10 + 5)
debug.upvaluejoin(foo3, 2, foo2, 1)
assert(foo3() == 10 + 5)
debug.upvaluejoin(foo3, 2, foo2, 2)
assert(foo3() == 10 + 3)

assert(not pcall(debug.upvaluejoin, foo1, 3, foo2, 1))
assert(not pcall(debug.upvaluejoin, foo1, 1, foo2, 3))
assert(not pcall(debug.upvaluejoin, foo1, 0, foo2, 1))
assert(not pcall(debug.upvaluejoin, print, 1, foo2, 1))
assert(not pcall(debug.upvaluejoin, {}, 1, foo2, 1))
assert(not pcall(debug.upvaluejoin, foo1, 1, print, 1))

print'OK'

-- $Id: testes/code.lua $
-- See Copyright Notice in file all.lua

if T==nil then
    (Message or print)('\n >>> testC not active: skipping opcode tests <<<\n')
    return
  end
  print "testing code generation and optimizations"
  
  -- to test constant propagation
  local k0aux <const> = 0
  local k0 <const> = k0aux
  local k1 <const> = 1
  local k3 <const> = 3
  local k6 <const> = k3 + (k3 << k0)
  local kFF0 <const> = 0xFF0
  local k3_78 <const> = 3.78
  local x, k3_78_4 <const> = 10, k3_78 / 4
  assert(x == 10)
  
  local kx <const> = "x"
  
  local kTrue <const> = true
  local kFalse <const> = false
  
  local kNil <const> = nil
  
  -- this code gave an error for the code checker
  do
    local function f (a)
    for k,v,w in a do end
    end
  end
  
  
  -- testing reuse in constant table
  local function checkKlist (func, list)
    local k = T.listk(func)
    assert(#k == #list)
    for i = 1, #k do
      assert(k[i] == list[i] and math.type(k[i]) == math.type(list[i]))
    end
  end
  
  local function foo ()
    local a
    a = k3;
    a = 0; a = 0.0; a = -7 + 7
    a = k3_78/4; a = k3_78_4
    a = -k3_78/4; a = k3_78/4; a = -3.78/4
    a = -3.79/4; a = 0.0; a = -0;
    a = k3; a = 3.0; a = 3; a = 3.0
  end
  
  checkKlist(foo, {3.78/4, -3.78/4, -3.79/4})
  
  
  foo = function (f, a)
          f(100 * 1000)
          f(100.0 * 1000)
          f(-100 * 1000)
          f(-100 * 1000.0)
          f(100000)
          f(100000.0)
          f(-100000)
          f(-100000.0)
        end
  
  checkKlist(foo, {100000, 100000.0, -100000, -100000.0})
  
  
  -- floats x integers
  foo = function (t, a)
    t[a] = 1; t[a] = 1.0
    t[a] = 1; t[a] = 1.0
    t[a] = 2; t[a] = 2.0
    t[a] = 0; t[a] = 0.0
    t[a] = 1; t[a] = 1.0
    t[a] = 2; t[a] = 2.0
    t[a] = 0; t[a] = 0.0
  end
  
  checkKlist(foo, {1, 1.0, 2, 2.0, 0, 0.0})
  
  
  -- testing opcodes
  
  -- check that 'f' opcodes match '...'
  function check (f, ...)
    local arg = {...}
    local c = T.listcode(f)
    for i=1, #arg do
      local opcode = string.match(c[i], "%u%w+")
      -- print(arg[i], opcode)
      assert(arg[i] == opcode)
    end
    assert(c[#arg+2] == undef)
  end
  
  
  -- check that 'f' opcodes match '...' and that 'f(p) == r'.
  function checkR (f, p, r, ...)
    local r1 = f(p)
    assert(r == r1 and math.type(r) == math.type(r1))
    check(f, ...)
  end
  
  
  -- check that 'a' and 'b' has the same opcodes
  function checkequal (a, b)
    a = T.listcode(a)
    b = T.listcode(b)
    assert(#a == #b)
    for i = 1, #a do
      a[i] = string.gsub(a[i], '%b()', '')   -- remove line number
      b[i] = string.gsub(b[i], '%b()', '')   -- remove line number
      assert(a[i] == b[i])
    end
  end
  
  
  -- some basic instructions
  check(function ()   -- function does not create upvalues
    (function () end){f()}
  end, 'CLOSURE', 'NEWTABLE', 'EXTRAARG', 'GETTABUP', 'CALL',
       'SETLIST', 'CALL', 'RETURN0')
  
  check(function (x)   -- function creates upvalues
    (function () return x end){f()}
  end, 'CLOSURE', 'NEWTABLE', 'EXTRAARG', 'GETTABUP', 'CALL',
       'SETLIST', 'CALL', 'RETURN')
  
  
  -- sequence of LOADNILs
  check(function ()
    local kNil <const> = nil
    local a,b,c
    local d; local e;
    local f,g,h;
    d = nil; d=nil; b=nil; a=kNil; c=nil;
  end, 'LOADNIL', 'RETURN0')
  
  check(function ()
    local a,b,c,d = 1,1,1,1
    d=nil;c=nil;b=nil;a=nil
  end, 'LOADI', 'LOADI', 'LOADI', 'LOADI', 'LOADNIL', 'RETURN0')
  
  do
    local a,b,c,d = 1,1,1,1
    d=nil;c=nil;b=nil;a=nil
    assert(a == nil and b == nil and c == nil and d == nil)
  end
  
  
  -- single return
  check (function (a,b,c) return a end, 'RETURN1')
  
  
  -- infinite loops
  check(function () while kTrue do local a = -1 end end,
  'LOADI', 'JMP', 'RETURN0')
  
  check(function () while 1 do local a = -1 end end,
  'LOADI', 'JMP', 'RETURN0')
  
  check(function () repeat local x = 1 until true end,
  'LOADI', 'RETURN0')
  
  
  -- concat optimization
  check(function (a,b,c,d) return a..b..c..d end,
    'MOVE', 'MOVE', 'MOVE', 'MOVE', 'CONCAT', 'RETURN1')
  
  -- not
  check(function () return not not nil end, 'LOADFALSE', 'RETURN1')
  check(function () return not not kFalse end, 'LOADFALSE', 'RETURN1')
  check(function () return not not true end, 'LOADTRUE', 'RETURN1')
  check(function () return not not k3 end, 'LOADTRUE', 'RETURN1')
  
  -- direct access to locals
  check(function ()
    local a,b,c,d
    a = b*a
    c.x, a[b] = -((a + d/b - a[b]) ^ a.x), b
  end,
    'LOADNIL',
    'MUL', 'MMBIN',
    'DIV', 'MMBIN', 'ADD', 'MMBIN', 'GETTABLE', 'SUB', 'MMBIN',
    'GETFIELD', 'POW', 'MMBIN', 'UNM', 'SETTABLE', 'SETFIELD', 'RETURN0')
  
  
  -- direct access to constants
  check(function ()
    local a,b
    local c = kNil
    a[kx] = 3.2
    a.x = b
    a[b] = 'x'
  end,
    'LOADNIL', 'SETFIELD', 'SETFIELD', 'SETTABLE', 'RETURN0')
  
  -- "get/set table" with numeric indices
  check(function (a)
    local k255 <const> = 255
    a[1] = a[100]
    a[k255] = a[256]
    a[256] = 5
  end,
    'GETI', 'SETI',
    'LOADI', 'GETTABLE', 'SETI',
    'LOADI', 'SETTABLE',  'RETURN0')
  
  check(function ()
    local a,b
    a = a - a
    b = a/a
    b = 5-4
  end,
    'LOADNIL', 'SUB', 'MMBIN', 'DIV', 'MMBIN', 'LOADI', 'RETURN0')
  
  check(function ()
    local a,b
    a[kTrue] = false
  end,
    'LOADNIL', 'LOADTRUE', 'SETTABLE', 'RETURN0')
  
  
  -- equalities
  checkR(function (a) if a == 1 then return 2 end end, 1, 2,
    'EQI', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if -4.0 == a then return 2 end end, -4, 2,
    'EQI', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if a == "hi" then return 2 end end, 10, nil,
    'EQK', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if a == 10000 then return 2 end end, 1, nil,
    'EQK', 'JMP', 'LOADI', 'RETURN1')   -- number too large
  
  checkR(function (a) if -10000 == a then return 2 end end, -10000, 2,
    'EQK', 'JMP', 'LOADI', 'RETURN1')   -- number too large
  
  -- comparisons
  
  checkR(function (a) if -10 <= a then return 2 end end, -10, 2,
    'GEI', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if 128.0 > a then return 2 end end, 129, nil,
    'LTI', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if -127.0 < a then return 2 end end, -127, nil,
    'GTI', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if 10 < a then return 2 end end, 11, 2,
    'GTI', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if 129 < a then return 2 end end, 130, 2,
    'LOADI', 'LT', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if a >= 23.0 then return 2 end end, 25, 2,
    'GEI', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if a >= 23.1 then return 2 end end, 0, nil,
    'LOADK', 'LE', 'JMP', 'LOADI', 'RETURN1')
  
  checkR(function (a) if a > 2300.0 then return 2 end end, 0, nil,
    'LOADF', 'LT', 'JMP', 'LOADI', 'RETURN1')
  
  
  -- constant folding
  local function checkK (func, val)
    check(func, 'LOADK', 'RETURN1')
    checkKlist(func, {val})
    assert(func() == val)
  end
  
  local function checkI (func, val)
    check(func, 'LOADI', 'RETURN1')
    checkKlist(func, {})
    assert(func() == val)
  end
  
  local function checkF (func, val)
    check(func, 'LOADF', 'RETURN1')
    checkKlist(func, {})
    assert(func() == val)
  end
  
  checkF(function () return 0.0 end, 0.0)
  checkI(function () return k0 end, 0)
  checkI(function () return -k0//1 end, 0)
  checkK(function () return 3^-1 end, 1/3)
  checkK(function () return (1 + 1)^(50 + 50) end, 2^100)
  checkK(function () return (-2)^(31 - 2) end, -0x20000000 + 0.0)
  checkF(function () return (-k3^0 + 5) // 3.0 end, 1.0)
  checkI(function () return -k3 % 5 end, 2)
  checkF(function () return -((2.0^8 + -(-1)) % 8)/2 * 4 - 3 end, -5.0)
  checkF(function () return -((2^8 + -(-1)) % 8)//2 * 4 - 3 end, -7.0)
  checkI(function () return 0xF0.0 | 0xCC.0 ~ 0xAA & 0xFD end, 0xF4)
  checkI(function () return ~(~kFF0 | kFF0) end, 0)
  checkI(function () return ~~-1024.0 end, -1024)
  checkI(function () return ((100 << k6) << -4) >> 2 end, 100)
  
  -- borders around MAXARG_sBx ((((1 << 17) - 1) >> 1) == 65535)
  local a = 17; local sbx = ((1 << a) - 1) >> 1   -- avoid folding
  local border <const> = 65535
  checkI(function () return border end, sbx)
  checkI(function () return -border end, -sbx)
  checkI(function () return border + 1 end, sbx + 1)
  checkK(function () return border + 2 end, sbx + 2)
  checkK(function () return -(border + 1) end, -(sbx + 1))
  
  local border <const> = 65535.0
  checkF(function () return border end, sbx + 0.0)
  checkF(function () return -border end, -sbx + 0.0)
  checkF(function () return border + 1 end, (sbx + 1.0))
  checkK(function () return border + 2 end, (sbx + 2.0))
  checkK(function () return -(border + 1) end, -(sbx + 1.0))
  
  
  -- immediate operands
  checkR(function (x) return x + k1 end, 10, 11, 'ADDI', 'MMBINI', 'RETURN1')
  checkR(function (x) return x - 127 end, 10, -117, 'ADDI', 'MMBINI', 'RETURN1')
  checkR(function (x) return 128 + x end, 0.0, 128.0,
           'ADDI', 'MMBINI', 'RETURN1')
  checkR(function (x) return x * -127 end, -1.0, 127.0,
           'MULK', 'MMBINK', 'RETURN1')
  checkR(function (x) return 20 * x end, 2, 40, 'MULK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x ^ -2 end, 2, 0.25, 'POWK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x / 40 end, 40, 1.0, 'DIVK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x // 1 end, 10.0, 10.0,
           'IDIVK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x % (100 - 10) end, 91, 1,
           'MODK', 'MMBINK', 'RETURN1')
  checkR(function (x) return k1 << x end, 3, 8, 'SHLI', 'MMBINI', 'RETURN1')
  checkR(function (x) return x << 127 end, 10, 0, 'SHRI', 'MMBINI', 'RETURN1')
  checkR(function (x) return x << -127 end, 10, 0, 'SHRI', 'MMBINI', 'RETURN1')
  checkR(function (x) return x >> 128 end, 8, 0, 'SHRI', 'MMBINI', 'RETURN1')
  checkR(function (x) return x >> -127 end, 8, 0, 'SHRI', 'MMBINI', 'RETURN1')
  checkR(function (x) return x & 1 end, 9, 1, 'BANDK', 'MMBINK', 'RETURN1')
  checkR(function (x) return 10 | x end, 1, 11, 'BORK', 'MMBINK', 'RETURN1')
  checkR(function (x) return -10 ~ x end, -1, 9, 'BXORK', 'MMBINK', 'RETURN1')
  
  -- K operands in arithmetic operations
  checkR(function (x) return x + 0.0 end, 1, 1.0, 'ADDK', 'MMBINK', 'RETURN1')
  --  check(function (x) return 128 + x end, 'ADDK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x * -10000 end, 2, -20000,
           'MULK', 'MMBINK', 'RETURN1')
  --  check(function (x) return 20 * x end, 'MULK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x ^ 0.5 end, 4, 2.0, 'POWK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x / 2.0 end, 4, 2.0, 'DIVK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x // 10000 end, 10000, 1,
           'IDIVK', 'MMBINK', 'RETURN1')
  checkR(function (x) return x % (100.0 - 10) end, 91, 1.0,
           'MODK', 'MMBINK', 'RETURN1')
  
  -- no foldings (and immediate operands)
  check(function () return -0.0 end, 'LOADF', 'UNM', 'RETURN1')
  check(function () return k3/0 end, 'LOADI', 'DIVK', 'MMBINK', 'RETURN1')
  check(function () return 0%0 end, 'LOADI', 'MODK', 'MMBINK', 'RETURN1')
  check(function () return -4//0 end, 'LOADI', 'IDIVK', 'MMBINK', 'RETURN1')
  check(function (x) return x >> 2.0 end, 'LOADF', 'SHR', 'MMBIN', 'RETURN1')
  check(function (x) return x << 128 end, 'LOADI', 'SHL', 'MMBIN', 'RETURN1')
  check(function (x) return x & 2.0 end, 'LOADF', 'BAND', 'MMBIN', 'RETURN1')
  
  -- basic 'for' loops
  check(function () for i = -10, 10.5 do end end,
  'LOADI', 'LOADK', 'LOADI', 'FORPREP', 'FORLOOP', 'RETURN0')
  check(function () for i = 0xfffffff, 10.0, 1 do end end,
  'LOADK', 'LOADF', 'LOADI', 'FORPREP', 'FORLOOP', 'RETURN0')
  
  -- bug in constant folding for 5.1
  check(function () return -nil end, 'LOADNIL', 'UNM', 'RETURN1')
  
  
  check(function ()
    local a,b,c
    b[c], a = c, b
    b[a], a = c, b
    a, b = c, a
    a = a
  end,
    'LOADNIL',
    'MOVE', 'MOVE', 'SETTABLE',
    'MOVE', 'MOVE', 'MOVE', 'SETTABLE',
    'MOVE', 'MOVE', 'MOVE',
    -- no code for a = a
    'RETURN0')
  
  
  -- x == nil , x ~= nil
  -- checkequal(function (b) if (a==nil) then a=1 end; if a~=nil then a=1 end end,
  --            function () if (a==9) then a=1 end; if a~=9 then a=1 end end)
  
  -- check(function () if a==nil then a='a' end end,
  -- 'GETTABUP', 'EQ', 'JMP', 'SETTABUP', 'RETURN')
  
  do   -- tests for table access in upvalues
    local t
    check(function () t[kx] = t.y end, 'GETTABUP', 'SETTABUP')
    check(function (a) t[a()] = t[a()] end,
    'MOVE', 'CALL', 'GETUPVAL', 'MOVE', 'CALL',
    'GETUPVAL', 'GETTABLE', 'SETTABLE')
  end
  
  -- de morgan
  checkequal(function () local a; if not (a or b) then b=a end end,
             function () local a; if (not a and not b) then b=a end end)
  
  checkequal(function (l) local a; return 0 <= a and a <= l end,
             function (l) local a; return not (not(a >= 0) or not(a <= l)) end)
  
  
  -- if-break optimizations
  check(function (a, b)
          while a do
            if b then break else a = a + 1 end
          end
        end,
  'TEST', 'JMP', 'TEST', 'JMP', 'ADDI', 'MMBINI', 'JMP', 'RETURN0')
  
  checkequal(function () return 6 or true or nil end,
             function () return k6 or kTrue or kNil end)
  
  checkequal(function () return 6 and true or nil end,
             function () return k6 and kTrue or kNil end)
  
  
  do   -- string constants
    local k0 <const> = "00000000000000000000000000000000000000000000000000"
    local function f1 ()
      local k <const> = k0
      return function ()
               return function () return k end
             end
    end
  
    local f2 = f1()
    local f3 = f2()
    assert(f3() == k0)
    checkK(f3, k0)
    -- string is not needed by other functions
    assert(T.listk(f1)[1] == nil)
    assert(T.listk(f2)[1] == nil)
  end
  
  print 'OK'
  
  -- $Id: testes/constructs.lua $
-- See Copyright Notice in file all.lua

;;print "testing syntax";;

local debug = require "debug"


local function checkload (s, msg)
  assert(string.find(select(2, load(s)), msg))
end

-- testing semicollons
do ;;; end
; do ; a = 3; assert(a == 3) end;
;


-- invalid operations should not raise errors when not executed
if false then a = 3 // 0; a = 0 % 0 end


-- testing priorities

assert(2^3^2 == 2^(3^2));
assert(2^3*4 == (2^3)*4);
assert(2.0^-2 == 1/4 and -2^- -2 == - - -4);
assert(not nil and 2 and not(2>3 or 3<2));
assert(-3-1-5 == 0+0-9);
assert(-2^2 == -4 and (-2)^2 == 4 and 2*2-3-1 == 0);
assert(-3%5 == 2 and -3+5 == 2)
assert(2*1+3/3 == 3 and 1+2 .. 3*1 == "33");
assert(not(2+1 > 3*1) and "a".."b" > "a");

assert(0xF0 | 0xCC ~ 0xAA & 0xFD == 0xF4)
assert(0xFD & 0xAA ~ 0xCC | 0xF0 == 0xF4)
assert(0xF0 & 0x0F + 1 == 0x10)

assert(3^4//2^3//5 == 2)

assert(-3+4*5//2^3^2//9+4%10/3 == (-3)+(((4*5)//(2^(3^2)))//9)+((4%10)/3))

assert(not ((true or false) and nil))
assert(      true or false  and nil)

-- old bug
assert((((1 or false) and true) or false) == true)
assert((((nil and true) or false) and true) == false)

local a,b = 1,nil;
assert(-(1 or 2) == -1 and (1 and 2)+(-1.25 or -4) == 0.75);
x = ((b or a)+1 == 2 and (10 or a)+1 == 11); assert(x);
x = (((2<3) or 1) == true and (2<3 and 4) == 4); assert(x);

x,y=1,2;
assert((x>y) and x or y == 2);
x,y=2,1;
assert((x>y) and x or y == 2);

assert(1234567890 == tonumber('1234567890') and 1234567890+1 == 1234567891)

do   -- testing operators with diffent kinds of constants
  -- operands to consider:
  --  * fit in register
  --  * constant doesn't fit in register
  --  * floats with integral values
  local operand = {3, 100, 5.0, -10, -5.0, 10000, -10000}
  local operator = {"+", "-", "*", "/", "//", "%", "^",
                    "&", "|", "^", "<<", ">>",
                    "==", "~=", "<", ">", "<=", ">=",}
  for _, op in ipairs(operator) do
    local f = assert(load(string.format([[return function (x,y)
                return x %s y
              end]], op)))();
    for _, o1 in ipairs(operand) do
      for _, o2 in ipairs(operand) do
        local gab = f(o1, o2)

        _ENV.XX = o1
        code = string.format("return XX %s %s", op, o2)
        res = assert(load(code))()
        assert(res == gab)

        _ENV.XX = o2
        local code = string.format("return (%s) %s XX", o1, op)
        local res = assert(load(code))()
        assert(res == gab)

        code = string.format("return (%s) %s %s", o1, op, o2)
        res = assert(load(code))()
        assert(res == gab)
      end
    end
  end
end


-- silly loops
repeat until 1; repeat until true;
while false do end; while nil do end;

do  -- test old bug (first name could not be an `upvalue')
 local a; function f(x) x={a=1}; x={x=1}; x={G=1} end
end

function f (i)
  if type(i) ~= 'number' then return i,'jojo'; end;
  if i > 0 then return i, f(i-1); end;
end

x = {f(3), f(5), f(10);};
assert(x[1] == 3 and x[2] == 5 and x[3] == 10 and x[4] == 9 and x[12] == 1);
assert(x[nil] == nil)
x = {f'alo', f'xixi', nil};
assert(x[1] == 'alo' and x[2] == 'xixi' and x[3] == nil);
x = {f'alo'..'xixi'};
assert(x[1] == 'aloxixi')
x = {f{}}
assert(x[2] == 'jojo' and type(x[1]) == 'table')


local f = function (i)
  if i < 10 then return 'a';
  elseif i < 20 then return 'b';
  elseif i < 30 then return 'c';
  end;
end

assert(f(3) == 'a' and f(12) == 'b' and f(26) == 'c' and f(100) == nil)

for i=1,1000 do break; end;
n=100;
i=3;
t = {};
a=nil
while not a do
  a=0; for i=1,n do for i=i,1,-1 do a=a+1; t[i]=1; end; end;
end
assert(a == n*(n+1)/2 and i==3);
assert(t[1] and t[n] and not t[0] and not t[n+1])

function f(b)
  local x = 1;
  repeat
    local a;
    if b==1 then local b=1; x=10; break
    elseif b==2 then x=20; break;
    elseif b==3 then x=30;
    else local a,b,c,d=math.sin(1); x=x+1;
    end
  until x>=12;
  return x;
end;

assert(f(1) == 10 and f(2) == 20 and f(3) == 30 and f(4)==12)


local f = function (i)
  if i < 10 then return 'a'
  elseif i < 20 then return 'b'
  elseif i < 30 then return 'c'
  else return 8
  end
end

assert(f(3) == 'a' and f(12) == 'b' and f(26) == 'c' and f(100) == 8)

local a, b = nil, 23
x = {f(100)*2+3 or a, a or b+2}
assert(x[1] == 19 and x[2] == 25)
x = {f=2+3 or a, a = b+2}
assert(x.f == 5 and x.a == 25)

a={y=1}
x = {a.y}
assert(x[1] == 1)

function f(i)
  while 1 do
    if i>0 then i=i-1;
    else return; end;
  end;
end;

function g(i)
  while 1 do
    if i>0 then i=i-1
    else return end
  end
end

f(10); g(10);

do
  function f () return 1,2,3; end
  local a, b, c = f();
  assert(a==1 and b==2 and c==3)
  a, b, c = (f());
  assert(a==1 and b==nil and c==nil)
end

local a,b = 3 and f();
assert(a==1 and b==nil)

function g() f(); return; end;
assert(g() == nil)
function g() return nil or f() end
a,b = g()
assert(a==1 and b==nil)

print'+';

do   -- testing constants
  local prog <const> = [[local x <XXX> = 10]]
  checkload(prog, "unknown attribute 'XXX'")

  checkload([[local xxx <const> = 20; xxx = 10]],
             ":1: attempt to assign to const variable 'xxx'")

  checkload([[
    local xx;
    local xxx <const> = 20;
    local yyy;
    local function foo ()
      local abc = xx + yyy + xxx;
      return function () return function () xxx = yyy end end
    end
  ]], ":6: attempt to assign to const variable 'xxx'")

  checkload([[
    local x <close> = nil
    x = io.open()
  ]], ":2: attempt to assign to const variable 'x'")
end

f = [[
return function ( a , b , c , d , e )
  local x = a >= b or c or ( d and e ) or nil
  return x
end , { a = 1 , b = 2 >= 1 , } or { 1 };
]]
f = string.gsub(f, "%s+", "\n");   -- force a SETLINE between opcodes
f,a = load(f)();
assert(a.a == 1 and a.b)

function g (a,b,c,d,e)
  if not (a>=b or c or d and e or nil) then return 0; else return 1; end;
end

function h (a,b,c,d,e)
  while (a>=b or c or (d and e) or nil) do return 1; end;
  return 0;
end;

assert(f(2,1) == true and g(2,1) == 1 and h(2,1) == 1)
assert(f(1,2,'a') == 'a' and g(1,2,'a') == 1 and h(1,2,'a') == 1)
assert(f(1,2,'a')
~=          -- force SETLINE before nil
nil, "")
assert(f(1,2,'a') == 'a' and g(1,2,'a') == 1 and h(1,2,'a') == 1)
assert(f(1,2,nil,1,'x') == 'x' and g(1,2,nil,1,'x') == 1 and
                                   h(1,2,nil,1,'x') == 1)
assert(f(1,2,nil,nil,'x') == nil and g(1,2,nil,nil,'x') == 0 and
                                     h(1,2,nil,nil,'x') == 0)
assert(f(1,2,nil,1,nil) == nil and g(1,2,nil,1,nil) == 0 and
                                   h(1,2,nil,1,nil) == 0)

assert(1 and 2<3 == true and 2<3 and 'a'<'b' == true)
x = 2<3 and not 3; assert(x==false)
x = 2<1 or (2>1 and 'a'); assert(x=='a')


do
  local a; if nil then a=1; else a=2; end;    -- this nil comes as PUSHNIL 2
  assert(a==2)
end

function F(a)
  assert(debug.getinfo(1, "n").name == 'F')
  return a,2,3
end

a,b = F(1)~=nil; assert(a == true and b == nil);
a,b = F(nil)==nil; assert(a == true and b == nil)

----------------------------------------------------------------
------------------------------------------------------------------

-- sometimes will be 0, sometimes will not...
_ENV.GLOB1 = math.random(0, 1)

-- basic expressions with their respective values
local basiccases = {
  {"nil", nil},
  {"false", false},
  {"true", true},
  {"10", 10},
  {"(0==_ENV.GLOB1)", 0 == _ENV.GLOB1},
}

local prog

if _ENV.GLOB1 == 0 then
  basiccases[2][1] = "F"   -- constant false

  prog = [[
    local F <const> = false
    if %s then IX = true end
    return %s
]]
else
  basiccases[4][1] = "k10"   -- constant 10

  prog = [[
    local k10 <const> = 10
    if %s then IX = true end
    return %s
  ]]
end

print('testing short-circuit optimizations (' .. _ENV.GLOB1 .. ')')


-- operators with their respective values
local binops <const> = {
  {" and ", function (a,b) if not a then return a else return b end end},
  {" or ", function (a,b) if a then return a else return b end end},
}

local cases <const> = {}

-- creates all combinations of '(cases[i] op cases[n-i])' plus
-- 'not(cases[i] op cases[n-i])' (syntax + value)
local function createcases (n)
  local res = {}
  for i = 1, n - 1 do
    for _, v1 in ipairs(cases[i]) do
      for _, v2 in ipairs(cases[n - i]) do
        for _, op in ipairs(binops) do
            local t = {
              "(" .. v1[1] .. op[1] .. v2[1] .. ")",
              op[2](v1[2], v2[2])
            }
            res[#res + 1] = t
            res[#res + 1] = {"not" .. t[1], not t[2]}
        end
      end
    end
  end
  return res
end

-- do not do too many combinations for soft tests
local level = _soft and 3 or 4

cases[1] = basiccases
for i = 2, level do cases[i] = createcases(i) end
print("+")

local i = 0
for n = 1, level do
  for _, v in pairs(cases[n]) do
    local s = v[1]
    local p = load(string.format(prog, s, s), "")
    IX = false
    assert(p() == v[2] and IX == not not v[2])
    i = i + 1
    if i % 60000 == 0 then print('+') end
  end
end
------------------------------------------------------------------

-- testing some syntax errors (chosen through 'gcov')
checkload("for x do", "expected")
checkload("x:call", "expected")

print'OK'
-- $Id: testes/coroutine.lua $
-- See Copyright Notice in file all.lua

print "testing coroutines"

local debug = require'debug'

local f

local main, ismain = coroutine.running()
assert(type(main) == "thread" and ismain)
assert(not coroutine.resume(main))
assert(not coroutine.isyieldable(main) and not coroutine.isyieldable())
assert(not pcall(coroutine.yield))


-- trivial errors
assert(not pcall(coroutine.resume, 0))
assert(not pcall(coroutine.status, 0))


-- tests for multiple yield/resume arguments

local function eqtab (t1, t2)
  assert(#t1 == #t2)
  for i = 1, #t1 do
    local v = t1[i]
    assert(t2[i] == v)
  end
end

_G.x = nil   -- declare x
function foo (a, ...)
  local x, y = coroutine.running()
  assert(x == f and y == false)
  -- next call should not corrupt coroutine (but must fail,
  -- as it attempts to resume the running coroutine)
  assert(coroutine.resume(f) == false)
  assert(coroutine.status(f) == "running")
  local arg = {...}
  assert(coroutine.isyieldable(x))
  for i=1,#arg do
    _G.x = {coroutine.yield(table.unpack(arg[i]))}
  end
  return table.unpack(a)
end

f = coroutine.create(foo)
assert(coroutine.isyieldable(f))
assert(type(f) == "thread" and coroutine.status(f) == "suspended")
assert(string.find(tostring(f), "thread"))
local s,a,b,c,d
s,a,b,c,d = coroutine.resume(f, {1,2,3}, {}, {1}, {'a', 'b', 'c'})
assert(coroutine.isyieldable(f))
assert(s and a == nil and coroutine.status(f) == "suspended")
s,a,b,c,d = coroutine.resume(f)
eqtab(_G.x, {})
assert(s and a == 1 and b == nil)
assert(coroutine.isyieldable(f))
s,a,b,c,d = coroutine.resume(f, 1, 2, 3)
eqtab(_G.x, {1, 2, 3})
assert(s and a == 'a' and b == 'b' and c == 'c' and d == nil)
s,a,b,c,d = coroutine.resume(f, "xuxu")
eqtab(_G.x, {"xuxu"})
assert(s and a == 1 and b == 2 and c == 3 and d == nil)
assert(coroutine.status(f) == "dead")
s, a = coroutine.resume(f, "xuxu")
assert(not s and string.find(a, "dead") and coroutine.status(f) == "dead")


-- yields in tail calls
local function foo (i) return coroutine.yield(i) end
f = coroutine.wrap(function ()
  for i=1,10 do
    assert(foo(i) == _G.x)
  end
  return 'a'
end)
for i=1,10 do _G.x = i; assert(f(i) == i) end
_G.x = 'xuxu'; assert(f('xuxu') == 'a')

-- recursive
function pf (n, i)
  coroutine.yield(n)
  pf(n*i, i+1)
end

f = coroutine.wrap(pf)
local s=1
for i=1,10 do
  assert(f(1, 1) == s)
  s = s*i
end

-- sieve
function gen (n)
  return coroutine.wrap(function ()
    for i=2,n do coroutine.yield(i) end
  end)
end


function filter (p, g)
  return coroutine.wrap(function ()
    while 1 do
      local n = g()
      if n == nil then return end
      if math.fmod(n, p) ~= 0 then coroutine.yield(n) end
    end
  end)
end

local x = gen(80)
local a = {}
while 1 do
  local n = x()
  if n == nil then break end
  table.insert(a, n)
  x = filter(n, x)
end

assert(#a == 22 and a[#a] == 79)
x, a = nil


print("to-be-closed variables in coroutines")

local function func2close (f)
  return setmetatable({}, {__close = f})
end

do
  -- ok to close a dead coroutine
  local co = coroutine.create(print)
  assert(coroutine.resume(co, "testing 'coroutine.close'"))
  assert(coroutine.status(co) == "dead")
  local st, msg = coroutine.close(co)
  assert(st and msg == nil)
  -- also ok to close it again
  st, msg = coroutine.close(co)
  assert(st and msg == nil)


  -- cannot close the running coroutine
  local st, msg = pcall(coroutine.close, coroutine.running())
  assert(not st and string.find(msg, "running"))

  local main = coroutine.running()

  -- cannot close a "normal" coroutine
  ;(coroutine.wrap(function ()
    local st, msg = pcall(coroutine.close, main)
    assert(not st and string.find(msg, "normal"))
  end))()

  -- cannot close a coroutine while closing it
  do
    local co
    co = coroutine.create(
      function()
        local x <close> = func2close(function()
            coroutine.close(co)   -- try to close it again
         end)
        coroutine.yield(20)
      end)
    local st, msg = coroutine.resume(co)
    assert(st and msg == 20)
    st, msg = coroutine.close(co)
    assert(not st and string.find(msg, "running coroutine"))
  end

  -- to-be-closed variables in coroutines
  local X

  -- closing a coroutine after an error
  local co = coroutine.create(error)
  local st, msg = coroutine.resume(co, 100)
  assert(not st and msg == 100)
  st, msg = coroutine.close(co)
  assert(not st and msg == 100)
  -- after closing, no more errors
  st, msg = coroutine.close(co)
  assert(st and msg == nil)

  co = coroutine.create(function ()
    local x <close> = func2close(function (self, err)
      assert(err == nil); X = false
    end)
    X = true
    coroutine.yield()
  end)
  coroutine.resume(co)
  assert(X)
  assert(coroutine.close(co))
  assert(not X and coroutine.status(co) == "dead")

  -- error closing a coroutine
  local x = 0
  co = coroutine.create(function()
    local y <close> = func2close(function (self,err)
      assert(err == 111)
      x = 200
      error(200)
    end)
    local x <close> = func2close(function (self, err)
      assert(err == nil); error(111)
    end)
    coroutine.yield()
  end)
  coroutine.resume(co)
  assert(x == 0)
  local st, msg = coroutine.close(co)
  assert(st == false and coroutine.status(co) == "dead" and msg == 200)
  assert(x == 200)
  -- after closing, no more errors
  st, msg = coroutine.close(co)
  assert(st and msg == nil)
end

do
  -- <close> versus pcall in coroutines
  local X = false
  local Y = false
  function foo ()
    local x <close> = func2close(function (self, err)
      Y = debug.getinfo(2)
      X = err
    end)
    error(43)
  end
  co = coroutine.create(function () return pcall(foo) end)
  local st1, st2, err = coroutine.resume(co)
  assert(st1 and not st2 and err == 43)
  assert(X == 43 and Y.what == "C")

  -- recovering from errors in __close metamethods
  local track = {}

  local function h (o)
    local hv <close> = o
    return 1
  end

  local function foo ()
    local x <close> = func2close(function(_,msg)
      track[#track + 1] = msg or false
      error(20)
    end)
    local y <close> = func2close(function(_,msg)
      track[#track + 1] = msg or false
      return 1000
    end)
    local z <close> = func2close(function(_,msg)
      track[#track + 1] = msg or false
      error(10)
    end)
    coroutine.yield(1)
    h(func2close(function(_,msg)
        track[#track + 1] = msg or false
        error(2)
      end))
  end

  local co = coroutine.create(pcall)

  local st, res = coroutine.resume(co, foo)    -- call 'foo' protected
  assert(st and res == 1)   -- yield 1
  local st, res1, res2 = coroutine.resume(co)   -- continue
  assert(coroutine.status(co) == "dead")
  assert(st and not res1 and res2 == 20)   -- last error (20)
  assert(track[1] == false and track[2] == 2 and track[3] == 10 and
         track[4] == 10)
end


-- yielding across C boundaries

co = coroutine.wrap(function()
       assert(not pcall(table.sort,{1,2,3}, coroutine.yield))
       assert(coroutine.isyieldable())
       coroutine.yield(20)
       return 30
     end)

assert(co() == 20)
assert(co() == 30)


local f = function (s, i) return coroutine.yield(i) end

local f1 = coroutine.wrap(function ()
             return xpcall(pcall, function (...) return ... end,
               function ()
                 local s = 0
                 for i in f, nil, 1 do pcall(function () s = s + i end) end
                 error({s})
               end)
           end)

f1()
for i = 1, 10 do assert(f1(i) == i) end
local r1, r2, v = f1(nil)
assert(r1 and not r2 and v[1] ==  (10 + 1)*10/2)


function f (a, b) a = coroutine.yield(a);  error{a + b} end
function g(x) return x[1]*2 end

co = coroutine.wrap(function ()
       coroutine.yield(xpcall(f, g, 10, 20))
     end)

assert(co() == 10)
r, msg = co(100)
assert(not r and msg == 240)


-- unyieldable C call
do
  local function f (c)
          assert(not coroutine.isyieldable())
          return c .. c
        end

  local co = coroutine.wrap(function (c)
               assert(coroutine.isyieldable())
               local s = string.gsub("a", ".", f)
               return s
             end)
  assert(co() == "aa")
end



do   -- testing single trace of coroutines
  local X
  local co = coroutine.create(function ()
    coroutine.yield(10)
    return 20;
  end)
  local trace = {}
  local function dotrace (event)
    trace[#trace + 1] = event
  end
  debug.sethook(co, dotrace, "clr")
  repeat until not coroutine.resume(co)
  local correcttrace = {"call", "line", "call", "return", "line", "return"}
  assert(#trace == #correcttrace)
  for k, v in pairs(trace) do
    assert(v == correcttrace[k])
  end
end

-- errors in coroutines
function foo ()
  assert(debug.getinfo(1).currentline == debug.getinfo(foo).linedefined + 1)
  assert(debug.getinfo(2).currentline == debug.getinfo(goo).linedefined)
  coroutine.yield(3)
  error(foo)
end

function goo() foo() end
x = coroutine.wrap(goo)
assert(x() == 3)
local a,b = pcall(x)
assert(not a and b == foo)

x = coroutine.create(goo)
a,b = coroutine.resume(x)
assert(a and b == 3)
a,b = coroutine.resume(x)
assert(not a and b == foo and coroutine.status(x) == "dead")
a,b = coroutine.resume(x)
assert(not a and string.find(b, "dead") and coroutine.status(x) == "dead")


-- co-routines x for loop
function all (a, n, k)
  if k == 0 then coroutine.yield(a)
  else
    for i=1,n do
      a[k] = i
      all(a, n, k-1)
    end
  end
end

local a = 0
for t in coroutine.wrap(function () all({}, 5, 4) end) do
  a = a+1
end
assert(a == 5^4)


-- access to locals of collected corroutines
local C = {}; setmetatable(C, {__mode = "kv"})
local x = coroutine.wrap (function ()
            local a = 10
            local function f () a = a+10; return a end
            while true do
              a = a+1
              coroutine.yield(f)
            end
          end)

C[1] = x;

local f = x()
assert(f() == 21 and x()() == 32 and x() == f)
x = nil
collectgarbage()
assert(C[1] == undef)
assert(f() == 43 and f() == 53)


-- old bug: attempt to resume itself

function co_func (current_co)
  assert(coroutine.running() == current_co)
  assert(coroutine.resume(current_co) == false)
  coroutine.yield(10, 20)
  assert(coroutine.resume(current_co) == false)
  coroutine.yield(23)
  return 10
end

local co = coroutine.create(co_func)
local a,b,c = coroutine.resume(co, co)
assert(a == true and b == 10 and c == 20)
a,b = coroutine.resume(co, co)
assert(a == true and b == 23)
a,b = coroutine.resume(co, co)
assert(a == true and b == 10)
assert(coroutine.resume(co, co) == false)
assert(coroutine.resume(co, co) == false)


-- other old bug when attempting to resume itself
-- (trigger C-code assertions)
do
  local A = coroutine.running()
  local B = coroutine.create(function() return coroutine.resume(A) end)
  local st, res = coroutine.resume(B)
  assert(st == true and res == false)

  local X = false
  A = coroutine.wrap(function()
    local _ <close> = func2close(function () X = true end)
    return pcall(A, 1)
  end)
  st, res = A()
  assert(not st and string.find(res, "non%-suspended") and X == true)
end


-- bug in 5.4.1
do
  -- coroutine ran close metamethods with invalid status during a
  -- reset.
  local co
  co = coroutine.wrap(function()
    local x <close> = func2close(function() return pcall(co) end)
    error(111)
  end)
  local st, errobj = pcall(co)
  assert(not st and errobj == 111)
  st, errobj = pcall(co)
  assert(not st and string.find(errobj, "dead coroutine"))
end


-- attempt to resume 'normal' coroutine
local co1, co2
co1 = coroutine.create(function () return co2() end)
co2 = coroutine.wrap(function ()
        assert(coroutine.status(co1) == 'normal')
        assert(not coroutine.resume(co1))
        coroutine.yield(3)
      end)

a,b = coroutine.resume(co1)
assert(a and b == 3)
assert(coroutine.status(co1) == 'dead')

-- infinite recursion of coroutines
a = function(a) coroutine.wrap(a)(a) end
assert(not pcall(a, a))
a = nil


-- access to locals of erroneous coroutines
local x = coroutine.create (function ()
            local a = 10
            _G.f = function () a=a+1; return a end
            error('x')
          end)

assert(not coroutine.resume(x))
-- overwrite previous position of local `a'
assert(not coroutine.resume(x, 1, 1, 1, 1, 1, 1, 1))
assert(_G.f() == 11)
assert(_G.f() == 12)


if not T then
  (Message or print)
      ('\n >>> testC not active: skipping coroutine API tests <<<\n')
else
  print "testing yields inside hooks"

  local turn
  
  function fact (t, x)
    assert(turn == t)
    if x == 0 then return 1
    else return x*fact(t, x-1)
    end
  end

  local A, B = 0, 0

  local x = coroutine.create(function ()
    T.sethook("yield 0", "", 2)
    A = fact("A", 6)
  end)

  local y = coroutine.create(function ()
    T.sethook("yield 0", "", 3)
    B = fact("B", 7)
  end)

  while A==0 or B==0 do    -- A ~= 0 when 'x' finishes (similar for 'B','y')
    if A==0 then turn = "A"; assert(T.resume(x)) end
    if B==0 then turn = "B"; assert(T.resume(y)) end

    -- check that traceback works correctly after yields inside hooks
    debug.traceback(x)
    debug.traceback(y)
  end

  assert(B // A == 7)    -- fact(7) // fact(6)

  do   -- hooks vs. multiple values
    local done
    local function test (n)
      done = false
      return coroutine.wrap(function ()
        local a = {}
        for i = 1, n do a[i] = i end
        -- 'pushint' just to perturb the stack
        T.sethook("pushint 10; yield 0", "", 1)   -- yield at each op.
        local a1 = {table.unpack(a)}   -- must keep top between ops.
        assert(#a1 == n)
        for i = 1, n do assert(a[i] == i) end
        done = true
      end)
    end
    -- arguments to the coroutine are just to perturb its stack
    local co = test(0); while not done do co(30) end
    co = test(1); while not done do co(20, 10) end
    co = test(3); while not done do co() end
    co = test(100); while not done do co() end
  end

  local line = debug.getinfo(1, "l").currentline + 2    -- get line number
  local function foo ()
    local x = 10    --<< this line is 'line'
    x = x + 10
    _G.XX = x
  end

  -- testing yields in line hook
  local co = coroutine.wrap(function ()
    T.sethook("setglobal X; yield 0", "l", 0); foo(); return 10 end)

  _G.XX = nil;
  _G.X = nil; co(); assert(_G.X == line)
  _G.X = nil; co(); assert(_G.X == line + 1)
  _G.X = nil; co(); assert(_G.X == line + 2 and _G.XX == nil)
  _G.X = nil; co(); assert(_G.X == line + 3 and _G.XX == 20)
  assert(co() == 10)

  -- testing yields in count hook
  co = coroutine.wrap(function ()
    T.sethook("yield 0", "", 1); foo(); return 10 end)

  _G.XX = nil;
  local c = 0
  repeat c = c + 1; local a = co() until a == 10
  assert(_G.XX == 20 and c >= 5)

  co = coroutine.wrap(function ()
    T.sethook("yield 0", "", 2); foo(); return 10 end)

  _G.XX = nil;
  local c = 0
  repeat c = c + 1; local a = co() until a == 10
  assert(_G.XX == 20 and c >= 5)
  _G.X = nil; _G.XX = nil

  do
    -- testing debug library on a coroutine suspended inside a hook
    -- (bug in 5.2/5.3)
    c = coroutine.create(function (a, ...)
      T.sethook("yield 0", "l")   -- will yield on next two lines
      assert(a == 10)
      return ...
    end)

    assert(coroutine.resume(c, 1, 2, 3))   -- start coroutine
    local n,v = debug.getlocal(c, 0, 1)    -- check its local
    assert(n == "a" and v == 1)
    assert(debug.setlocal(c, 0, 1, 10))     -- test 'setlocal'
    local t = debug.getinfo(c, 0)        -- test 'getinfo'
    assert(t.currentline == t.linedefined + 1)
    assert(not debug.getinfo(c, 1))      -- no other level
    assert(coroutine.resume(c))          -- run next line
    v = {coroutine.resume(c)}         -- finish coroutine
    assert(v[1] == true and v[2] == 2 and v[3] == 3 and v[4] == undef)
    assert(not coroutine.resume(c))
  end

  do
    -- testing debug library on last function in a suspended coroutine
    -- (bug in 5.2/5.3)
    local c = coroutine.create(function () T.testC("yield 1", 10, 20) end)
    local a, b = coroutine.resume(c)
    assert(a and b == 20)
    assert(debug.getinfo(c, 0).linedefined == -1)
    a, b = debug.getlocal(c, 0, 2)
    assert(b == 10)
  end


  print "testing coroutine API"
  
  -- reusing a thread
  assert(T.testC([[
    newthread      # create thread
    pushvalue 2    # push body
    pushstring 'a a a'  # push argument
    xmove 0 3 2   # move values to new thread
    resume -1, 1    # call it first time
    pushstatus
    xmove 3 0 0   # move results back to stack
    setglobal X    # result
    setglobal Y    # status
    pushvalue 2     # push body (to call it again)
    pushstring 'b b b'
    xmove 0 3 2
    resume -1, 1    # call it again
    pushstatus
    xmove 3 0 0
    return 1        # return result
  ]], function (...) return ... end) == 'b b b')

  assert(X == 'a a a' and Y == 'OK')


  -- resuming running coroutine
  C = coroutine.create(function ()
        return T.testC([[
                 pushnum 10;
                 pushnum 20;
                 resume -3 2;
                 pushstatus
                 gettop;
                 return 3]], C)
      end)
  local a, b, c, d = coroutine.resume(C)
  assert(a == true and string.find(b, "non%-suspended") and
         c == "ERRRUN" and d == 4)

  a, b, c, d = T.testC([[
    rawgeti R 1    # get main thread
    pushnum 10;
    pushnum 20;
    resume -3 2;
    pushstatus
    gettop;
    return 4]])
  assert(a == coroutine.running() and string.find(b, "non%-suspended") and
         c == "ERRRUN" and d == 4)


  -- using a main thread as a coroutine  (dubious use!)
  local state = T.newstate()

  -- check that yielddable is working correctly
  assert(T.testC(state, "newthread; isyieldable -1; remove 1; return 1"))

  -- main thread is not yieldable
  assert(not T.testC(state, "rawgeti R 1; isyieldable -1; remove 1; return 1"))

  T.testC(state, "settop 0")

  T.loadlib(state)

  assert(T.doremote(state, [[
    coroutine = require'coroutine';
    X = function (x) coroutine.yield(x, 'BB'); return 'CC' end;
    return 'ok']]))

  t = table.pack(T.testC(state, [[
    rawgeti R 1     # get main thread
    pushstring 'XX'
    getglobal X    # get function for body
    pushstring AA      # arg
    resume 1 1      # 'resume' shadows previous stack!
    gettop
    setglobal T    # top
    setglobal B    # second yielded value
    setglobal A    # fist yielded value
    rawgeti R 1     # get main thread
    pushnum 5       # arg (noise)
    resume 1 1      # after coroutine ends, previous stack is back
    pushstatus
    return *
  ]]))
  assert(t.n == 4 and t[2] == 'XX' and t[3] == 'CC' and t[4] == 'OK')
  assert(T.doremote(state, "return T") == '2')
  assert(T.doremote(state, "return A") == 'AA')
  assert(T.doremote(state, "return B") == 'BB')

  T.closestate(state)

  print'+'

end


-- leaving a pending coroutine open
_X = coroutine.wrap(function ()
      local a = 10
      local x = function () a = a+1 end
      coroutine.yield()
    end)

_X()


if not _soft then
  -- bug (stack overflow)
  local j = 2^9
  local lim = 1000000    -- (C stack limit; assume 32-bit machine)
  local t = {lim - 10, lim - 5, lim - 1, lim, lim + 1}
  for i = 1, #t do
    local j = t[i]
    co = coroutine.create(function()
           local t = {}
           for i = 1, j do t[i] = i end
           return table.unpack(t)
         end)
    local r, msg = coroutine.resume(co)
    assert(not r)
  end
  co = nil
end


assert(coroutine.running() == main)

print"+"


print"testing yields inside metamethods"

local function val(x)
  if type(x) == "table" then return x.x else return x end
end

local mt = {
  __eq = function(a,b) coroutine.yield(nil, "eq"); return val(a) == val(b) end,
  __lt = function(a,b) coroutine.yield(nil, "lt"); return val(a) < val(b) end,
  __le = function(a,b) coroutine.yield(nil, "le"); return a - b <= 0 end,
  __add = function(a,b) coroutine.yield(nil, "add");
                        return val(a) + val(b) end,
  __sub = function(a,b) coroutine.yield(nil, "sub"); return val(a) - val(b) end,
  __mul = function(a,b) coroutine.yield(nil, "mul"); return val(a) * val(b) end,
  __div = function(a,b) coroutine.yield(nil, "div"); return val(a) / val(b) end,
  __idiv = function(a,b) coroutine.yield(nil, "idiv");
                         return val(a) // val(b) end,
  __pow = function(a,b) coroutine.yield(nil, "pow"); return val(a) ^ val(b) end,
  __mod = function(a,b) coroutine.yield(nil, "mod"); return val(a) % val(b) end,
  __unm = function(a,b) coroutine.yield(nil, "unm"); return -val(a) end,
  __bnot = function(a,b) coroutine.yield(nil, "bnot"); return ~val(a) end,
  __shl = function(a,b) coroutine.yield(nil, "shl");
                        return val(a) << val(b) end,
  __shr = function(a,b) coroutine.yield(nil, "shr");
                        return val(a) >> val(b) end,
  __band = function(a,b)
             coroutine.yield(nil, "band")
             return val(a) & val(b)
           end,
  __bor = function(a,b) coroutine.yield(nil, "bor");
                        return val(a) | val(b) end,
  __bxor = function(a,b) coroutine.yield(nil, "bxor");
                         return val(a) ~ val(b) end,

  __concat = function(a,b)
               coroutine.yield(nil, "concat");
               return val(a) .. val(b)
             end,
  __index = function (t,k) coroutine.yield(nil, "idx"); return t.k[k] end,
  __newindex = function (t,k,v) coroutine.yield(nil, "nidx"); t.k[k] = v end,
}


local function new (x)
  return setmetatable({x = x, k = {}}, mt)
end


local a = new(10)
local b = new(12)
local c = new"hello"

local function run (f, t)
  local i = 1
  local c = coroutine.wrap(f)
  while true do
    local res, stat = c()
    if res then assert(t[i] == undef); return res, t end
    assert(stat == t[i])
    i = i + 1
  end
end


assert(run(function () if (a>=b) then return '>=' else return '<' end end,
       {"le", "sub"}) == "<")
assert(run(function () if (a<=b) then return '<=' else return '>' end end,
       {"le", "sub"}) == "<=")
assert(run(function () if (a==b) then return '==' else return '~=' end end,
       {"eq"}) == "~=")

assert(run(function () return a & b + a end, {"add", "band"}) == 2)

assert(run(function () return 1 + a end, {"add"}) == 11)
assert(run(function () return a - 25 end, {"sub"}) == -15)
assert(run(function () return 2 * a end, {"mul"}) == 20)
assert(run(function () return a ^ 2 end, {"pow"}) == 100)
assert(run(function () return a / 2 end, {"div"}) == 5)
assert(run(function () return a % 6 end, {"mod"}) == 4)
assert(run(function () return a // 3 end, {"idiv"}) == 3)

assert(run(function () return a + b end, {"add"}) == 22)
assert(run(function () return a - b end, {"sub"}) == -2)
assert(run(function () return a * b end, {"mul"}) == 120)
assert(run(function () return a ^ b end, {"pow"}) == 10^12)
assert(run(function () return a / b end, {"div"}) == 10/12)
assert(run(function () return a % b end, {"mod"}) == 10)
assert(run(function () return a // b end, {"idiv"}) == 0)

-- repeat tests with larger constants (to use 'K' opcodes)
local a1000 = new(1000)

assert(run(function () return a1000 + 1000 end, {"add"}) == 2000)
assert(run(function () return a1000 - 25000 end, {"sub"}) == -24000)
assert(run(function () return 2000 * a end, {"mul"}) == 20000)
assert(run(function () return a1000 / 1000 end, {"div"}) == 1)
assert(run(function () return a1000 % 600 end, {"mod"}) == 400)
assert(run(function () return a1000 // 500 end, {"idiv"}) == 2)



assert(run(function () return a % b end, {"mod"}) == 10)

assert(run(function () return ~a & b end, {"bnot", "band"}) == ~10 & 12)
assert(run(function () return a | b end, {"bor"}) == 10 | 12)
assert(run(function () return a ~ b end, {"bxor"}) == 10 ~ 12)
assert(run(function () return a << b end, {"shl"}) == 10 << 12)
assert(run(function () return a >> b end, {"shr"}) == 10 >> 12)

assert(run(function () return 10 & b end, {"band"}) == 10 & 12)
assert(run(function () return a | 2 end, {"bor"}) == 10 | 2)
assert(run(function () return a ~ 2 end, {"bxor"}) == 10 ~ 2)
assert(run(function () return a >> 2 end, {"shr"}) == 10 >> 2)
assert(run(function () return 1 >> a end, {"shr"}) == 1 >> 10)
assert(run(function () return a << 2 end, {"shl"}) == 10 << 2)
assert(run(function () return 1 << a end, {"shl"}) == 1 << 10)
assert(run(function () return 2 ~ a end, {"bxor"}) == 2 ~ 10)


assert(run(function () return a..b end, {"concat"}) == "1012")

assert(run(function() return a .. b .. c .. a end,
       {"concat", "concat", "concat"}) == "1012hello10")

assert(run(function() return "a" .. "b" .. a .. "c" .. c .. b .. "x" end,
       {"concat", "concat", "concat"}) == "ab10chello12x")


do   -- a few more tests for comparison operators
  local mt1 = {
    __le = function (a,b)
      coroutine.yield(10)
      return (val(a) <= val(b))
    end,
    __lt = function (a,b)
      coroutine.yield(10)
      return val(a) < val(b)
    end,
  }
  local mt2 = { __lt = mt1.__lt, __le = mt1.__le }

  local function run (f)
    local co = coroutine.wrap(f)
    local res
    repeat
      res = co()
    until res ~= 10
    return res
  end
  
  local function test ()
    local a1 = setmetatable({x=1}, mt1)
    local a2 = setmetatable({x=2}, mt2)
    assert(a1 < a2)
    assert(a1 <= a2)
    assert(1 < a2)
    assert(1 <= a2)
    assert(2 > a1)
    assert(2 >= a2)
    return true
  end
  
  run(test)

end

assert(run(function ()
             a.BB = print
             return a.BB
           end, {"nidx", "idx"}) == print)

-- getuptable & setuptable
do local _ENV = _ENV
  f = function () AAA = BBB + 1; return AAA end
end
g = new(10); g.k.BBB = 10;
debug.setupvalue(f, 1, g)
assert(run(f, {"idx", "nidx", "idx"}) == 11)
assert(g.k.AAA == 11)

print"+"

print"testing yields inside 'for' iterators"

local f = function (s, i)
      if i%2 == 0 then coroutine.yield(nil, "for") end
      if i < s then return i + 1 end
    end

assert(run(function ()
             local s = 0
             for i in f, 4, 0 do s = s + i end
             return s
           end, {"for", "for", "for"}) == 10)



-- tests for coroutine API
if T==nil then
  (Message or print)('\n >>> testC not active: skipping coroutine API tests <<<\n')
  print "OK"; return
end

print('testing coroutine API')

local function apico (...)
  local x = {...}
  return coroutine.wrap(function ()
    return T.testC(table.unpack(x))
  end)
end

local a = {apico(
[[
  pushstring errorcode
  pcallk 1 0 2;
  invalid command (should not arrive here)
]],
[[return *]],
"stackmark",
error
)()}
assert(#a == 4 and
       a[3] == "stackmark" and
       a[4] == "errorcode" and
       _G.status == "ERRRUN" and
       _G.ctx == 2)       -- 'ctx' to pcallk

local co = apico(
  "pushvalue 2; pushnum 10; pcallk 1 2 3; invalid command;",
  coroutine.yield,
  "getglobal status; getglobal ctx; pushvalue 2; pushstring a; pcallk 1 0 4; invalid command",
  "getglobal status; getglobal ctx; return *")

assert(co() == 10)
assert(co(20, 30) == 'a')
a = {co()}
assert(#a == 10 and
       a[2] == coroutine.yield and
       a[5] == 20 and a[6] == 30 and
       a[7] == "YIELD" and a[8] == 3 and
       a[9] == "YIELD" and a[10] == 4)
assert(not pcall(co))   -- coroutine is dead now


f = T.makeCfunc("pushnum 3; pushnum 5; yield 1;")
co = coroutine.wrap(function ()
  assert(f() == 23); assert(f() == 23); return 10
end)
assert(co(23,16) == 5)
assert(co(23,16) == 5)
assert(co(23,16) == 10)


-- testing coroutines with C bodies
f = T.makeCfunc([[
        pushnum 102
	yieldk	1 U2
	cannot be here!
]],
[[      # continuation
	pushvalue U3   # accessing upvalues inside a continuation
        pushvalue U4
	return *
]], 23, "huu")

x = coroutine.wrap(f)
assert(x() == 102)
eqtab({x()}, {23, "huu"})


f = T.makeCfunc[[pushstring 'a'; pushnum 102; yield 2; ]]

a, b, c, d = T.testC([[newthread; pushvalue 2; xmove 0 3 1; resume 3 0;
                       pushstatus; xmove 3 0 0;  resume 3 0; pushstatus;
                       return 4; ]], f)

assert(a == 'YIELD' and b == 'a' and c == 102 and d == 'OK')


-- testing chain of suspendable C calls

local count = 3   -- number of levels

f = T.makeCfunc([[
  remove 1;             # remove argument
  pushvalue U3;         # get selection function
  call 0 1;             # call it  (result is 'f' or 'yield')
  pushstring hello      # single argument for selected function
  pushupvalueindex 2;   # index of continuation program
  callk 1 -1 .;		# call selected function
  errorerror		# should never arrive here
]],
[[
  # continuation program
  pushnum 34	# return value
  return *     # return all results
]],
function ()     -- selection function
  count = count - 1
  if count == 0 then return coroutine.yield
  else return f
  end
end
)

co = coroutine.wrap(function () return f(nil) end)
assert(co() == "hello")   -- argument to 'yield'
a = {co()}
-- three '34's (one from each pending C call)
assert(#a == 3 and a[1] == a[2] and a[2] == a[3] and a[3] == 34)


-- testing yields with continuations

co = coroutine.wrap(function (...) return
       T.testC([[ # initial function
          yieldk 1 2
          cannot be here!
       ]],
       [[  # 1st continuation
         yieldk 0 3 
         cannot be here!
       ]],
       [[  # 2nd continuation
         yieldk 0 4 
         cannot be here!
       ]],
       [[  # 3th continuation
          pushvalue 6   # function which is last arg. to 'testC' here
          pushnum 10; pushnum 20;
          pcall 2 0 0   # call should throw an error and return to next line
          pop 1		# remove error message
          pushvalue 6
          getglobal status; getglobal ctx
          pcallk 2 2 5  # call should throw an error and jump to continuation
          cannot be here!
       ]],
       [[  # 4th (and last) continuation
         return *
       ]],
       -- function called by 3th continuation
       function (a,b) x=a; y=b; error("errmsg") end,
       ...
)
end)

local a = {co(3,4,6)}
assert(a[1] == 6 and a[2] == undef)
a = {co()}; assert(a[1] == undef and _G.status == "YIELD" and _G.ctx == 2)
a = {co()}; assert(a[1] == undef and _G.status == "YIELD" and _G.ctx == 3)
a = {co(7,8)};
-- original arguments
assert(type(a[1]) == 'string' and type(a[2]) == 'string' and
     type(a[3]) == 'string' and type(a[4]) == 'string' and
     type(a[5]) == 'string' and type(a[6]) == 'function')
-- arguments left from fist resume
assert(a[7] == 3 and a[8] == 4)
-- arguments to last resume
assert(a[9] == 7 and a[10] == 8)
-- error message and nothing more
assert(a[11]:find("errmsg") and #a == 11)
-- check arguments to pcallk
assert(x == "YIELD" and y == 4)

assert(not pcall(co))   -- coroutine should be dead


-- bug in nCcalls
local co = coroutine.wrap(function ()
  local a = {pcall(pcall,pcall,pcall,pcall,pcall,pcall,pcall,error,"hi")}
  return pcall(assert, table.unpack(a))
end)

local a = {co()}
assert(a[10] == "hi")

print'OK'

-- $Id: testes/cstack.lua $
-- See Copyright Notice in file all.lua


local tracegc = require"tracegc"

print"testing stack overflow detection"

-- Segmentation faults in these tests probably result from a C-stack
-- overflow. To avoid these errors, you should set a smaller limit for
-- the use of C stack by Lua, by changing the constant 'LUAI_MAXCCALLS'.
-- Alternatively, you can ensure a larger stack for the program.


local function checkerror (msg, f, ...)
  local s, err = pcall(f, ...)
  assert(not s and string.find(err, msg))
end

do  print("testing stack overflow in message handling")
  local count = 0
  local function loop (x, y, z)
    count = count + 1
    return 1 + loop(x, y, z)
  end
  tracegc.stop()    -- __gc should not be called with a full stack
  local res, msg = xpcall(loop, loop)
  tracegc.start()
  assert(msg == "error in error handling")
  print("final count: ", count)
end


-- bug since 2.5 (C-stack overflow in recursion inside pattern matching)
do  print("testing recursion inside pattern matching")
  local function f (size)
    local s = string.rep("a", size)
    local p = string.rep(".?", size)
    return string.match(s, p)
  end
  local m = f(80)
  assert(#m == 80)
  checkerror("too complex", f, 2000)
end


do  print("testing stack-overflow in recursive 'gsub'")
  local count = 0
  local function foo ()
    count = count + 1
    string.gsub("a", ".", foo)
  end
  checkerror("stack overflow", foo)
  print("final count: ", count)

  print("testing stack-overflow in recursive 'gsub' with metatables")
  local count = 0
  local t = setmetatable({}, {__index = foo})
  foo = function ()
    count = count + 1
    string.gsub("a", ".", t)
  end
  checkerror("stack overflow", foo)
  print("final count: ", count)
end


do   -- bug in 5.4.0
  print("testing limits in coroutines inside deep calls")
  local count = 0
  local lim = 1000
  local function stack (n)
    if n > 0 then return stack(n - 1) + 1
    else coroutine.wrap(function ()
           count = count + 1
           stack(lim)
         end)()
    end
  end

  local st, msg = xpcall(stack, function () return "ok" end, lim)
  assert(not st and msg == "ok")
  print("final count: ", count)
end


do
  print("nesting of resuming yielded coroutines")
  local count = 0

  local function body ()
    coroutine.yield()
    local f = coroutine.wrap(body)
    f();  -- start new coroutine (will stop in previous yield)
    count = count + 1
    f()   -- call it recursively
  end

  local f = coroutine.wrap(body)
  f()
  assert(not pcall(f))
  print("final count: ", count)
end


do    -- bug in 5.4.2
  print("nesting coroutines running after recoverable errors")
  local count = 0
  local function foo()
    count = count + 1
    pcall(1)   -- create an error
    -- running now inside 'precover' ("protected recover")
    coroutine.wrap(foo)()   -- call another coroutine
  end
  checkerror("C stack overflow", foo)
  print("final count: ", count)
end


if T then
  print("testing stack recovery")
  local N = 0      -- trace number of calls
  local LIM = -1   -- will store N just before stack overflow

  -- trace stack size; after stack overflow, it should be
  -- the maximum allowed stack size.
  local stack1
  local dummy

  local function err(msg)
    assert(string.find(msg, "stack overflow"))
    local _, stacknow = T.stacklevel()
    assert(stacknow == stack1 + 200)
  end

  -- When LIM==-1, the 'if' is not executed, so this function only
  -- counts and stores the stack limits up to overflow.  Then, LIM
  -- becomes N, and then the 'if' code is run when the stack is
  -- full. Then, there is a stack overflow inside 'xpcall', after which
  -- the stack must have been restored back to its maximum normal size.
  local function f()
    dummy, stack1 = T.stacklevel()
    if N == LIM then
      xpcall(f, err)
      local _, stacknow = T.stacklevel()
      assert(stacknow == stack1)
      return
    end
    N = N + 1
    f()
  end

  local topB, sizeB   -- top and size Before overflow
  local topA, sizeA   -- top and size After overflow
  topB, sizeB = T.stacklevel()
  tracegc.stop()    -- __gc should not be called with a full stack
  xpcall(f, err)
  tracegc.start()
  topA, sizeA = T.stacklevel()
  -- sizes should be comparable
  assert(topA == topB and sizeA < sizeB * 2)
  print(string.format("maximum stack size: %d", stack1))
  LIM = N      -- will stop recursion at maximum level
  N = 0        -- to count again
  tracegc.stop()    -- __gc should not be called with a full stack
  f()
  tracegc.start()
  print"+"
end

print'OK'

-- $Id: testes/db.lua $
-- See Copyright Notice in file all.lua

-- testing debug library

local debug = require "debug"

local function dostring(s) return assert(load(s))() end

print"testing debug library and debug information"

do
local a=1
end

assert(not debug.gethook())

local testline = 19         -- line where 'test' is defined
function test (s, l, p)     -- this must be line 19
  collectgarbage()   -- avoid gc during trace
  local function f (event, line)
    assert(event == 'line')
    local l = table.remove(l, 1)
    if p then print(l, line) end
    assert(l == line, "wrong trace!!")
  end
  debug.sethook(f,"l"); load(s)(); debug.sethook()
  assert(#l == 0)
end


do
  assert(not pcall(debug.getinfo, print, "X"))   -- invalid option
  assert(not pcall(debug.getinfo, 0, ">"))   -- invalid option
  assert(not debug.getinfo(1000))   -- out of range level
  assert(not debug.getinfo(-1))     -- out of range level
  local a = debug.getinfo(print)
  assert(a.what == "C" and a.short_src == "[C]")
  a = debug.getinfo(print, "L")
  assert(a.activelines == nil)
  local b = debug.getinfo(test, "SfL")
  assert(b.name == nil and b.what == "Lua" and b.linedefined == testline and
         b.lastlinedefined == b.linedefined + 10 and
         b.func == test and not string.find(b.short_src, "%["))
  assert(b.activelines[b.linedefined + 1] and
         b.activelines[b.lastlinedefined])
  assert(not b.activelines[b.linedefined] and
         not b.activelines[b.lastlinedefined + 1])
end


-- test file and string names truncation
a = "function f () end"
local function dostring (s, x) return load(s, x)() end
dostring(a)
assert(debug.getinfo(f).short_src == string.format('[string "%s"]', a))
dostring(a..string.format("; %s\n=1", string.rep('p', 400)))
assert(string.find(debug.getinfo(f).short_src, '^%[string [^\n]*%.%.%."%]$'))
dostring(a..string.format("; %s=1", string.rep('p', 400)))
assert(string.find(debug.getinfo(f).short_src, '^%[string [^\n]*%.%.%."%]$'))
dostring("\n"..a)
assert(debug.getinfo(f).short_src == '[string "..."]')
dostring(a, "")
assert(debug.getinfo(f).short_src == '[string ""]')
dostring(a, "@xuxu")
assert(debug.getinfo(f).short_src == "xuxu")
dostring(a, "@"..string.rep('p', 1000)..'t')
assert(string.find(debug.getinfo(f).short_src, "^%.%.%.p*t$"))
dostring(a, "=xuxu")
assert(debug.getinfo(f).short_src == "xuxu")
dostring(a, string.format("=%s", string.rep('x', 500)))
assert(string.find(debug.getinfo(f).short_src, "^x*$"))
dostring(a, "=")
assert(debug.getinfo(f).short_src == "")
a = nil; f = nil;


repeat
  local g = {x = function ()
    local a = debug.getinfo(2)
    assert(a.name == 'f' and a.namewhat == 'local')
    a = debug.getinfo(1)
    assert(a.name == 'x' and a.namewhat == 'field')
    return 'xixi'
  end}
  local f = function () return 1+1 and (not 1 or g.x()) end
  assert(f() == 'xixi')
  g = debug.getinfo(f)
  assert(g.what == "Lua" and g.func == f and g.namewhat == "" and not g.name)

  function f (x, name)   -- local!
    name = name or 'f'
    local a = debug.getinfo(1)
    assert(a.name == name and a.namewhat == 'local')
    return x
  end

  -- breaks in different conditions
  if 3>4 then break end; f()
  if 3<4 then a=1 else break end; f()
  while 1 do local x=10; break end; f()
  local b = 1
  if 3>4 then return math.sin(1) end; f()
  a = 3<4; f()
  a = 3<4 or 1; f()
  repeat local x=20; if 4>3 then f() else break end; f() until 1
  g = {}
  f(g).x = f(2) and f(10)+f(9)
  assert(g.x == f(19))
  function g(x) if not x then return 3 end return (x('a', 'x')) end
  assert(g(f) == 'a')
until 1

test([[if
math.sin(1)
then
  a=1
else
  a=2
end
]], {2,3,4,7})

test([[
local function foo()
end
foo()
A = 1
A = 2
A = 3
]], {2, 3, 2, 4, 5, 6})


test([[--
if nil then
  a=1
else
  a=2
end
]], {2,5,6})

test([[a=1
repeat
  a=a+1
until a==3
]], {1,3,4,3,4})

test([[ do
  return
end
]], {2})

test([[local a
a=1
while a<=3 do
  a=a+1
end
]], {1,2,3,4,3,4,3,4,3,5})

test([[while math.sin(1) do
  if math.sin(1)
  then break
  end
end
a=1]], {1,2,3,6})

test([[for i=1,3 do
  a=i
end
]], {1,2,1,2,1,2,1,3})

test([[for i,v in pairs{'a','b'} do
  a=tostring(i) .. v
end
]], {1,2,1,2,1,3})

test([[for i=1,4 do a=1 end]], {1,1,1,1})


do   -- testing line info/trace with large gaps in source

  local a = {1, 2, 3, 10, 124, 125, 126, 127, 128, 129, 130,
             255, 256, 257, 500, 1000}
  local s = [[
     local b = {10}
     a = b[1] X + Y b[1]
     b = 4
  ]]
  for _, i in ipairs(a) do
    local subs = {X = string.rep("\n", i)}
    for _, j in ipairs(a) do
      subs.Y = string.rep("\n", j)
      local s = string.gsub(s, "[XY]", subs)
      test(s, {1, 2 + i, 2 + i + j, 2 + i, 2 + i + j, 3 + i + j})
    end
  end
end


do   -- testing active lines
  local function checkactivelines (f, lines)
    local t = debug.getinfo(f, "SL")
    for _, l in pairs(lines) do
      l = l + t.linedefined
      assert(t.activelines[l])
      t.activelines[l] = undef
    end
    assert(next(t.activelines) == nil)   -- no extra lines
  end

  checkactivelines(function (...)   -- vararg function
    -- 1st line is empty
    -- 2nd line is empty
    -- 3th line is empty
    local a = 20
    -- 5th line is empty
    local b = 30
    -- 7th line is empty
  end, {4, 6, 8})

  checkactivelines(function (a)
    -- 1st line is empty
    -- 2nd line is empty
    local a = 20
    local b = 30
    -- 5th line is empty
  end, {3, 4, 6})

  checkactivelines(function (a, b, ...) end, {0})

  checkactivelines(function (a, b)
  end, {1})

  for _, n in pairs{0, 1, 2, 10, 50, 100, 1000, 10000} do
    checkactivelines(
      load(string.format("%s return 1", string.rep("\n", n))),
      {n + 1})
  end

end

print'+'

-- invalid levels in [gs]etlocal
assert(not pcall(debug.getlocal, 20, 1))
assert(not pcall(debug.setlocal, -1, 1, 10))


-- parameter names
local function foo (a,b,...) local d, e end
local co = coroutine.create(foo)

assert(debug.getlocal(foo, 1) == 'a')
assert(debug.getlocal(foo, 2) == 'b')
assert(not debug.getlocal(foo, 3))
assert(debug.getlocal(co, foo, 1) == 'a')
assert(debug.getlocal(co, foo, 2) == 'b')
assert(not debug.getlocal(co, foo, 3))

assert(not debug.getlocal(print, 1))


local function foo () return (debug.getlocal(1, -1)) end
assert(not foo(10))


-- varargs
local function foo (a, ...)
  local t = table.pack(...)
  for i = 1, t.n do
    local n, v = debug.getlocal(1, -i)
    assert(n == "(vararg)" and v == t[i])
  end
  assert(not debug.getlocal(1, -(t.n + 1)))
  assert(not debug.setlocal(1, -(t.n + 1), 30))
  if t.n > 0 then
    (function (x)
      assert(debug.setlocal(2, -1, x) == "(vararg)")
      assert(debug.setlocal(2, -t.n, x) == "(vararg)")
     end)(430)
     assert(... == 430)
  end
end

foo()
foo(print)
foo(200, 3, 4)
local a = {}
for i = 1, (_soft and 100 or 1000) do a[i] = i end
foo(table.unpack(a))
a = nil



do   -- test hook presence in debug info
  assert(not debug.gethook())
  local count = 0
  local function f ()
    assert(debug.getinfo(1).namewhat == "hook")
    local sndline = string.match(debug.traceback(), "\n(.-)\n")
    assert(string.find(sndline, "hook"))
    count = count + 1
  end
  debug.sethook(f, "l")
  local a = 0
  _ENV.a = a
  a = 1
  debug.sethook()
  assert(count == 4)
end


-- hook table has weak keys
assert(getmetatable(debug.getregistry()._HOOKKEY).__mode == 'k')


a = {}; L = nil
local glob = 1
local oldglob = glob
debug.sethook(function (e,l)
  collectgarbage()   -- force GC during a hook
  local f, m, c = debug.gethook()
  assert(m == 'crl' and c == 0)
  if e == "line" then
    if glob ~= oldglob then
      L = l-1   -- get the first line where "glob" has changed
      oldglob = glob
    end
  elseif e == "call" then
      local f = debug.getinfo(2, "f").func
      a[f] = 1
  else assert(e == "return")
  end
end, "crl")


function f(a,b)
  collectgarbage()
  local _, x = debug.getlocal(1, 1)
  local _, y = debug.getlocal(1, 2)
  assert(x == a and y == b)
  assert(debug.setlocal(2, 3, "pera") == "AA".."AA")
  assert(debug.setlocal(2, 4, "ma��") == "B")
  x = debug.getinfo(2)
  assert(x.func == g and x.what == "Lua" and x.name == 'g' and
         x.nups == 2 and string.find(x.source, "^@.*db%.lua$"))
  glob = glob+1
  assert(debug.getinfo(1, "l").currentline == L+1)
  assert(debug.getinfo(1, "l").currentline == L+2)
end

function foo()
  glob = glob+1
  assert(debug.getinfo(1, "l").currentline == L+1)
end; foo()  -- set L
-- check line counting inside strings and empty lines

_ = 'alo\
alo' .. [[

]]
--[[
]]
assert(debug.getinfo(1, "l").currentline == L+11)  -- check count of lines


function g (...)
  local arg = {...}
  do local a,b,c; a=math.sin(40); end
  local feijao
  local AAAA,B = "xuxu", "mam�o"
  f(AAAA,B)
  assert(AAAA == "pera" and B == "ma��")
  do
     local B = 13
     local x,y = debug.getlocal(1,5)
     assert(x == 'B' and y == 13)
  end
end

g()


assert(a[f] and a[g] and a[assert] and a[debug.getlocal] and not a[print])


-- tests for manipulating non-registered locals (C and Lua temporaries)

local n, v = debug.getlocal(0, 1)
assert(v == 0 and n == "(C temporary)")
local n, v = debug.getlocal(0, 2)
assert(v == 2 and n == "(C temporary)")
assert(not debug.getlocal(0, 3))
assert(not debug.getlocal(0, 0))

function f()
  assert(select(2, debug.getlocal(2,3)) == 1)
  assert(not debug.getlocal(2,4))
  debug.setlocal(2, 3, 10)
  return 20
end

function g(a,b) return (a+1) + f() end

assert(g(0,0) == 30)
 

debug.sethook(nil);
assert(not debug.gethook())


-- minimal tests for setuservalue/getuservalue
do
  assert(not debug.setuservalue(io.stdin, 10))
  local a, b = debug.getuservalue(io.stdin, 10)
  assert(a == nil and not b)
end

-- testing iteraction between multiple values x hooks
do
  local function f(...) return 3, ... end
  local count = 0
  local a = {}
  for i = 1, 100 do a[i] = i end
  debug.sethook(function () count = count + 1 end, "", 1)
  local t = {table.unpack(a)}
  assert(#t == 100)
  t = {table.unpack(a, 1, 3)}
  assert(#t == 3)
  t = {f(table.unpack(a, 1, 30))}
  assert(#t == 31)
end


-- testing access to function arguments

local function collectlocals (level)
  local tab = {}
  for i = 1, math.huge do
    local n, v = debug.getlocal(level + 1, i)
    if not (n and string.find(n, "^[a-zA-Z0-9_]+$")) then
       break   -- consider only real variables
    end
    tab[n] = v
  end
  return tab
end


X = nil
a = {}
function a:f (a, b, ...) local arg = {...}; local c = 13 end
debug.sethook(function (e)
  assert(e == "call")
  dostring("XX = 12")  -- test dostring inside hooks
  -- testing errors inside hooks
  assert(not pcall(load("a='joao'+1")))
  debug.sethook(function (e, l) 
    assert(debug.getinfo(2, "l").currentline == l)
    local f,m,c = debug.gethook()
    assert(e == "line")
    assert(m == 'l' and c == 0)
    debug.sethook(nil)  -- hook is called only once
    assert(not X)       -- check that
    X = collectlocals(2)
  end, "l")
end, "c")

a:f(1,2,3,4,5)
assert(X.self == a and X.a == 1   and X.b == 2 and X.c == nil)
assert(XX == 12)
assert(not debug.gethook())


-- testing access to local variables in return hook (bug in 5.2)
do
  local X = false

  local function foo (a, b, ...)
    do local x,y,z end
    local c, d = 10, 20
    return
  end

  local function aux ()
    if debug.getinfo(2).name == "foo" then
      X = true   -- to signal that it found 'foo'
      local tab = {a = 100, b = 200, c = 10, d = 20}
      for n, v in pairs(collectlocals(2)) do
        assert(tab[n] == v)
        tab[n] = undef
      end
      assert(next(tab) == nil)    -- 'tab' must be empty
    end
  end

  debug.sethook(aux, "r"); foo(100, 200); debug.sethook()
  assert(X)

end


local function eqseq (t1, t2)
  assert(#t1 == #t2)
  for i = 1, #t1 do
    assert(t1[i] == t2[i])
  end
end


do  print("testing inspection of parameters/returned values")
  local on = false
  local inp, out

  local function hook (event)
    if not on then return end
    local ar = debug.getinfo(2, "ruS")
    local t = {}
    for i = ar.ftransfer, ar.ftransfer + ar.ntransfer - 1 do
      local _, v = debug.getlocal(2, i)
      t[#t + 1] = v 
    end
    if event == "return" then
      out = t
    else
      inp = t
    end
  end

  debug.sethook(hook, "cr")

  on = true; math.sin(3); on = false
  eqseq(inp, {3}); eqseq(out, {math.sin(3)})

  on = true; select(2, 10, 20, 30, 40); on = false
  eqseq(inp, {2, 10, 20, 30, 40}); eqseq(out, {20, 30, 40})

  local function foo (a, ...) return ... end
  local function foo1 () on = not on; return foo(20, 10, 0) end
  foo1(); on = false
  eqseq(inp, {20}); eqseq(out, {10, 0})

  debug.sethook()
end



-- testing upvalue access
local function getupvalues (f)
  local t = {}
  local i = 1
  while true do
    local name, value = debug.getupvalue(f, i)
    if not name then break end
    assert(not t[name])
    t[name] = value
    i = i + 1
  end
  return t
end

local a,b,c = 1,2,3
local function foo1 (a) b = a; return c end
local function foo2 (x) a = x; return c+b end
assert(not debug.getupvalue(foo1, 3))
assert(not debug.getupvalue(foo1, 0))
assert(not debug.setupvalue(foo1, 3, "xuxu"))
local t = getupvalues(foo1)
assert(t.a == nil and t.b == 2 and t.c == 3)
t = getupvalues(foo2)
assert(t.a == 1 and t.b == 2 and t.c == 3)
assert(debug.setupvalue(foo1, 1, "xuxu") == "b")
assert(({debug.getupvalue(foo2, 3)})[2] == "xuxu")
-- upvalues of C functions are allways "called" "" (the empty string)
assert(debug.getupvalue(string.gmatch("x", "x"), 1) == "")  


-- testing count hooks
local a=0
debug.sethook(function (e) a=a+1 end, "", 1)
a=0; for i=1,1000 do end; assert(1000 < a and a < 1012)
debug.sethook(function (e) a=a+1 end, "", 4)
a=0; for i=1,1000 do end; assert(250 < a and a < 255)
local f,m,c = debug.gethook()
assert(m == "" and c == 4)
debug.sethook(function (e) a=a+1 end, "", 4000)
a=0; for i=1,1000 do end; assert(a == 0)

do
  debug.sethook(print, "", 2^24 - 1)   -- count upperbound
  local f,m,c = debug.gethook()
  assert(({debug.gethook()})[3] == 2^24 - 1)
end

debug.sethook()


-- tests for tail calls
local function f (x)
  if x then
    assert(debug.getinfo(1, "S").what == "Lua")
    assert(debug.getinfo(1, "t").istailcall == true)
    local tail = debug.getinfo(2)
    assert(tail.func == g1 and tail.istailcall == true)
    assert(debug.getinfo(3, "S").what == "main")
    print"+"
    end
end

function g(x) return f(x) end

function g1(x) g(x) end

local function h (x) local f=g1; return f(x) end

h(true)

local b = {}
debug.sethook(function (e) table.insert(b, e) end, "cr")
h(false)
debug.sethook()
local res = {"return",   -- first return (from sethook)
  "call", "tail call", "call", "tail call",
  "return", "return",
  "call",    -- last call (to sethook)
}
for i = 1, #res do assert(res[i] == table.remove(b, 1)) end

b = 0
debug.sethook(function (e)
                if e == "tail call" then
                  b = b + 1
                  assert(debug.getinfo(2, "t").istailcall == true)
                else
                  assert(debug.getinfo(2, "t").istailcall == false)
                end
              end, "c")
h(false)
debug.sethook()
assert(b == 2)   -- two tail calls

lim = _soft and 3000 or 30000
local function foo (x)
  if x==0 then
    assert(debug.getinfo(2).what == "main")
    local info = debug.getinfo(1)
    assert(info.istailcall == true and info.func == foo)
  else return foo(x-1)
  end
end

foo(lim)


print"+"


-- testing local function information
co = load[[
  local A = function ()
    return x
  end
  return
]]

local a = 0
-- 'A' should be visible to debugger only after its complete definition
debug.sethook(function (e, l)
  if l == 3 then a = a + 1; assert(debug.getlocal(2, 1) == "(temporary)")
  elseif l == 4 then a = a + 1; assert(debug.getlocal(2, 1) == "A")
  end
end, "l")
co()  -- run local function definition
debug.sethook()  -- turn off hook
assert(a == 2)   -- ensure all two lines where hooked

-- testing traceback

assert(debug.traceback(print) == print)
assert(debug.traceback(print, 4) == print)
assert(string.find(debug.traceback("hi", 4), "^hi\n"))
assert(string.find(debug.traceback("hi"), "^hi\n"))
assert(not string.find(debug.traceback("hi"), "'debug.traceback'"))
assert(string.find(debug.traceback("hi", 0), "'debug.traceback'"))
assert(string.find(debug.traceback(), "^stack traceback:\n"))

do  -- C-function names in traceback
  local st, msg = (function () return pcall end)()(debug.traceback)
  assert(st == true and string.find(msg, "pcall"))
end


-- testing nparams, nups e isvararg
local t = debug.getinfo(print, "u")
assert(t.isvararg == true and t.nparams == 0 and t.nups == 0)

t = debug.getinfo(function (a,b,c) end, "u")
assert(t.isvararg == false and t.nparams == 3 and t.nups == 0)

t = debug.getinfo(function (a,b,...) return t[a] end, "u")
assert(t.isvararg == true and t.nparams == 2 and t.nups == 1)

t = debug.getinfo(1)   -- main
assert(t.isvararg == true and t.nparams == 0 and t.nups == 1 and
       debug.getupvalue(t.func, 1) == "_ENV")

t = debug.getinfo(math.sin)   -- C function
assert(t.isvararg == true and t.nparams == 0 and t.nups == 0)

t = debug.getinfo(string.gmatch("abc", "a"))   -- C closure
assert(t.isvararg == true and t.nparams == 0 and t.nups > 0)



-- testing debugging of coroutines

local function checktraceback (co, p, level)
  local tb = debug.traceback(co, nil, level)
  local i = 0
  for l in string.gmatch(tb, "[^\n]+\n?") do
    assert(i == 0 or string.find(l, p[i]))
    i = i+1
  end
  assert(p[i] == undef)
end


local function f (n)
  if n > 0 then f(n-1)
  else coroutine.yield() end
end

local co = coroutine.create(f)
coroutine.resume(co, 3)
checktraceback(co, {"yield", "db.lua", "db.lua", "db.lua", "db.lua"})
checktraceback(co, {"db.lua", "db.lua", "db.lua", "db.lua"}, 1)
checktraceback(co, {"db.lua", "db.lua", "db.lua"}, 2)
checktraceback(co, {"db.lua"}, 4)
checktraceback(co, {}, 40)


co = coroutine.create(function (x)
       local a = 1
       coroutine.yield(debug.getinfo(1, "l"))
       coroutine.yield(debug.getinfo(1, "l").currentline)
       return a
     end)

local tr = {}
local foo = function (e, l) if l then table.insert(tr, l) end end
debug.sethook(co, foo, "lcr")

local _, l = coroutine.resume(co, 10)
local x = debug.getinfo(co, 1, "lfLS")
assert(x.currentline == l.currentline and x.activelines[x.currentline])
assert(type(x.func) == "function")
for i=x.linedefined + 1, x.lastlinedefined do
  assert(x.activelines[i])
  x.activelines[i] = undef
end
assert(next(x.activelines) == nil)   -- no 'extra' elements
assert(not debug.getinfo(co, 2))
local a,b = debug.getlocal(co, 1, 1)
assert(a == "x" and b == 10)
a,b = debug.getlocal(co, 1, 2)
assert(a == "a" and b == 1)
debug.setlocal(co, 1, 2, "hi")
assert(debug.gethook(co) == foo)
assert(#tr == 2 and
       tr[1] == l.currentline-1 and tr[2] == l.currentline)

a,b,c = pcall(coroutine.resume, co)
assert(a and b and c == l.currentline+1)
checktraceback(co, {"yield", "in function <"})

a,b = coroutine.resume(co)
assert(a and b == "hi")
assert(#tr == 4 and tr[4] == l.currentline+2)
assert(debug.gethook(co) == foo)
assert(not debug.gethook())
checktraceback(co, {})


-- check get/setlocal in coroutines
co = coroutine.create(function (x)
  local a, b = coroutine.yield(x)
  assert(a == 100 and b == nil)
  return x
end)
a, b = coroutine.resume(co, 10)
assert(a and b == 10)
a, b = debug.getlocal(co, 1, 1)
assert(a == "x" and b == 10)
assert(not debug.getlocal(co, 1, 5))
assert(debug.setlocal(co, 1, 1, 30) == "x")
assert(not debug.setlocal(co, 1, 5, 40))
a, b = coroutine.resume(co, 100)
assert(a and b == 30)


-- check traceback of suspended (or dead with error) coroutines

function f(i)
  if i == 0 then error(i)
  else coroutine.yield(); f(i-1)
  end
end


co = coroutine.create(function (x) f(x) end)
a, b = coroutine.resume(co, 3)
t = {"'coroutine.yield'", "'f'", "in function <"}
while coroutine.status(co) == "suspended" do
  checktraceback(co, t)
  a, b = coroutine.resume(co)
  table.insert(t, 2, "'f'")   -- one more recursive call to 'f'
end
t[1] = "'error'"
checktraceback(co, t)


-- test acessing line numbers of a coroutine from a resume inside
-- a C function (this is a known bug in Lua 5.0)

local function g(x)
    coroutine.yield(x)
end

local function f (i)
  debug.sethook(function () end, "l")
  for j=1,1000 do
    g(i+j)
  end
end

local co = coroutine.wrap(f)
co(10)
pcall(co)
pcall(co)


assert(type(debug.getregistry()) == "table")


-- test tagmethod information
local a = {}
local function f (t)
  local info = debug.getinfo(1);
  assert(info.namewhat == "metamethod")
  a.op = info.name
  return info.name
end
setmetatable(a, {
  __index = f; __add = f; __div = f; __mod = f; __concat = f; __pow = f;
  __mul = f; __idiv = f; __unm = f; __len = f; __sub = f;
  __shl = f; __shr = f; __bor = f; __bxor = f;
  __eq = f; __le = f; __lt = f; __unm = f; __len = f; __band = f;
  __bnot = f;
})

local b = setmetatable({}, getmetatable(a))

assert(a[3] == "index" and a^3 == "pow" and a..a == "concat")
assert(a/3 == "div" and 3%a == "mod")
assert(a+3 == "add" and 3-a == "sub" and a*3 == "mul" and
       -a == "unm" and #a == "len" and a&3 == "band")
assert(a + 30000 == "add" and a - 3.0 == "sub" and a * 3.0 == "mul" and
       -a == "unm" and #a == "len" and a & 3 == "band")
assert(a|3 == "bor" and 3~a == "bxor" and a<<3 == "shl" and a>>1 == "shr")
assert (a==b and a.op == "eq")
assert (a>=b and a.op == "le")
assert ("x">=a and a.op == "le")
assert (a>b and a.op == "lt")
assert (a>10 and a.op == "lt")
assert(~a == "bnot")

do   -- testing for-iterator name
  local function f()
    assert(debug.getinfo(1).name == "for iterator")
  end

  for i in f do end
end


do   -- testing debug info for finalizers
  local name = nil

  -- create a piece of garbage with a finalizer
  setmetatable({}, {__gc = function ()
    local t = debug.getinfo(2)   -- get callee information
    assert(t.namewhat == "metamethod")
    name = t.name
  end})

  -- repeat until previous finalizer runs (setting 'name')
  repeat local a = {} until name
  assert(name == "__gc")
end


do
  print("testing traceback sizes")

  local function countlines (s)
    return select(2, string.gsub(s, "\n", ""))
  end

  local function deep (lvl, n)
    if lvl == 0 then
      return (debug.traceback("message", n))
    else
      return (deep(lvl-1, n))
    end
  end

  local function checkdeep (total, start)
    local s = deep(total, start)
    local rest = string.match(s, "^message\nstack traceback:\n(.*)$")
    local cl = countlines(rest)
    -- at most 10 lines in first part, 11 in second, plus '...'
    assert(cl <= 10 + 11 + 1)
    local brk = string.find(rest, "%.%.%.")
    if brk then   -- does message have '...'?
      local rest1 = string.sub(rest, 1, brk)
      local rest2 = string.sub(rest, brk, #rest)
      assert(countlines(rest1) == 10 and countlines(rest2) == 11)
    else
      assert(cl == total - start + 2)
    end
  end

  for d = 1, 51, 10 do
    for l = 1, d do
      -- use coroutines to ensure complete control of the stack
      coroutine.wrap(checkdeep)(d, l)
    end
  end

end


print("testing debug functions on chunk without debug info")
prog = [[-- program to be loaded without debug information (strip)
local debug = require'debug'
local a = 12  -- a local variable

local n, v = debug.getlocal(1, 1)
assert(n == "(temporary)" and v == debug)   -- unkown name but known value
n, v = debug.getlocal(1, 2)
assert(n == "(temporary)" and v == 12)   -- unkown name but known value

-- a function with an upvalue
local f = function () local x; return a end
n, v = debug.getupvalue(f, 1)
assert(n == "(no name)" and v == 12)
assert(debug.setupvalue(f, 1, 13) == "(no name)")
assert(a == 13)

local t = debug.getinfo(f)
assert(t.name == nil and t.linedefined > 0 and
       t.lastlinedefined == t.linedefined and
       t.short_src == "?")
assert(debug.getinfo(1).currentline == -1)

t = debug.getinfo(f, "L").activelines
assert(next(t) == nil)    -- active lines are empty

-- dump/load a function without debug info
f = load(string.dump(f))

t = debug.getinfo(f)
assert(t.name == nil and t.linedefined > 0 and
       t.lastlinedefined == t.linedefined and
       t.short_src == "?")
assert(debug.getinfo(1).currentline == -1)

return a
]]


-- load 'prog' without debug info
local f = assert(load(string.dump(load(prog), true)))

assert(f() == 13)

do   -- bug in 5.4.0: line hooks in stripped code
  local function foo ()
    local a = 1
    local b = 2
    return b
  end

  local s = load(string.dump(foo, true))
  local line = true
  debug.sethook(function (e, l)
    assert(e == "line")
    line = l
  end, "l")
  assert(s() == 2); debug.sethook(nil)
  assert(line == nil)  -- hook called withoug debug info for 1st instruction
end

do   -- tests for 'source' in binary dumps
  local prog = [[
    return function (x)
      return function (y) 
        return x + y
      end
    end
  ]]
  local name = string.rep("x", 1000)
  local p = assert(load(prog, name))
  -- load 'p' as a binary chunk with debug information
  local c = string.dump(p)
  assert(#c > 1000 and #c < 2000)   -- no repetition of 'source' in dump
  local f = assert(load(c))
  local g = f()
  local h = g(3)
  assert(h(5) == 8)
  assert(debug.getinfo(f).source == name and   -- all functions have 'source'
         debug.getinfo(g).source == name and 
         debug.getinfo(h).source == name)
  -- again, without debug info
  local c = string.dump(p, true)
  assert(#c < 500)   -- no 'source' in dump
  local f = assert(load(c))
  local g = f()
  local h = g(30)
  assert(h(50) == 80)
  assert(debug.getinfo(f).source == '=?' and   -- no function has 'source'
         debug.getinfo(g).source == '=?' and 
         debug.getinfo(h).source == '=?')
end

print"OK"

-- $Id: testes/errors.lua $
-- See Copyright Notice in file all.lua

print("testing errors")

local debug = require"debug"

-- avoid problems with 'strict' module (which may generate other error messages)
local mt = getmetatable(_G) or {}
local oldmm = mt.__index
mt.__index = nil

local function checkerr (msg, f, ...)
  local st, err = pcall(f, ...)
  assert(not st and string.find(err, msg))
end


local function doit (s)
  local f, msg = load(s)
  if not f then return msg end
  local cond, msg = pcall(f)
  return (not cond) and msg
end


local function checkmessage (prog, msg, debug)
  local m = doit(prog)
  if debug then print(m, msg) end
  assert(string.find(m, msg, 1, true))
end

local function checksyntax (prog, extra, token, line)
  local msg = doit(prog)
  if not string.find(token, "^<%a") and not string.find(token, "^char%(")
    then token = "'"..token.."'" end
  token = string.gsub(token, "(%p)", "%%%1")
  local pt = string.format([[^%%[string ".*"%%]:%d: .- near %s$]],
                           line, token)
  assert(string.find(msg, pt))
  assert(string.find(msg, msg, 1, true))
end


-- test error message with no extra info
assert(doit("error('hi', 0)") == 'hi')

-- test error message with no info
assert(doit("error()") == nil)


-- test common errors/errors that crashed in the past
assert(doit("table.unpack({}, 1, n=2^30)"))
assert(doit("a=math.sin()"))
assert(not doit("tostring(1)") and doit("tostring()"))
assert(doit"tonumber()")
assert(doit"repeat until 1; a")
assert(doit"return;;")
assert(doit"assert(false)")
assert(doit"assert(nil)")
assert(doit("function a (... , ...) end"))
assert(doit("function a (, ...) end"))
assert(doit("local t={}; t = t[#t] + 1"))

checksyntax([[
  local a = {4

]], "'}' expected (to close '{' at line 1)", "<eof>", 3)


do   -- testing errors in goto/break
  local function checksyntax (prog, msg, line)
    local st, err = load(prog)
    assert(string.find(err, "line " .. line))
    assert(string.find(err, msg, 1, true))
  end

  checksyntax([[
    ::A:: a = 1
    ::A::
  ]], "label 'A' already defined", 1)

  checksyntax([[
    a = 1
    goto A
    do ::A:: end
  ]], "no visible label 'A'", 2)

end


if not T then
  (Message or print)
    ('\n >>> testC not active: skipping memory message test <<<\n')
else
  print "testing memory error message"
  local a = {}
  for i = 1, 10000 do a[i] = true end   -- preallocate array
  collectgarbage()
  T.totalmem(T.totalmem() + 10000)
  -- force a memory error (by a small margin)
  local st, msg = pcall(function()
    for i = 1, 100000 do a[i] = tostring(i) end
  end)
  T.totalmem(0)
  assert(not st and msg == "not enough" .. " memory")
end


-- tests for better error messages

checkmessage("a = {} + 1", "arithmetic")
checkmessage("a = {} | 1", "bitwise operation")
checkmessage("a = {} < 1", "attempt to compare")
checkmessage("a = {} <= 1", "attempt to compare")

checkmessage("a=1; bbbb=2; a=math.sin(3)+bbbb(3)", "global 'bbbb'")
checkmessage("a={}; do local a=1 end a:bbbb(3)", "method 'bbbb'")
checkmessage("local a={}; a.bbbb(3)", "field 'bbbb'")
assert(not string.find(doit"a={13}; local bbbb=1; a[bbbb](3)", "'bbbb'"))
checkmessage("a={13}; local bbbb=1; a[bbbb](3)", "number")
checkmessage("a=(1)..{}", "a table value")

-- calls
checkmessage("local a; a(13)", "local 'a'")
checkmessage([[
  local a = setmetatable({}, {__add = 34})
  a = a + 1
]], "metamethod 'add'")
checkmessage([[
  local a = setmetatable({}, {__lt = {}})
  a = a > a
]], "metamethod 'lt'")

-- tail calls
checkmessage("local a={}; return a.bbbb(3)", "field 'bbbb'")
checkmessage("a={}; do local a=1 end; return a:bbbb(3)", "method 'bbbb'")

checkmessage("a = #print", "length of a function value")
checkmessage("a = #3", "length of a number value")

aaa = nil
checkmessage("aaa.bbb:ddd(9)", "global 'aaa'")
checkmessage("local aaa={bbb=1}; aaa.bbb:ddd(9)", "field 'bbb'")
checkmessage("local aaa={bbb={}}; aaa.bbb:ddd(9)", "method 'ddd'")
checkmessage("local a,b,c; (function () a = b+1.1 end)()", "upvalue 'b'")
assert(not doit"local aaa={bbb={ddd=next}}; aaa.bbb:ddd(nil)")

-- upvalues being indexed do not go to the stack
checkmessage("local a,b,cc; (function () a = cc[1] end)()", "upvalue 'cc'")
checkmessage("local a,b,cc; (function () a.x = 1 end)()", "upvalue 'a'")

checkmessage("local _ENV = {x={}}; a = a + 1", "global 'a'")

checkmessage("b=1; local aaa={}; x=aaa+b", "local 'aaa'")
checkmessage("aaa={}; x=3.3/aaa", "global 'aaa'")
checkmessage("aaa=2; b=nil;x=aaa*b", "global 'b'")
checkmessage("aaa={}; x=-aaa", "global 'aaa'")

-- short circuit
checkmessage("a=1; local a,bbbb=2,3; a = math.sin(1) and bbbb(3)",
       "local 'bbbb'")
checkmessage("a=1; local a,bbbb=2,3; a = bbbb(1) or a(3)", "local 'bbbb'")
checkmessage("local a,b,c,f = 1,1,1; f((a and b) or c)", "local 'f'")
checkmessage("local a,b,c = 1,1,1; ((a and b) or c)()", "call a number value")
assert(not string.find(doit"aaa={}; x=(aaa or aaa)+(aaa and aaa)", "'aaa'"))
assert(not string.find(doit"aaa={}; (aaa or aaa)()", "'aaa'"))

checkmessage("print(print < 10)", "function with number")
checkmessage("print(print < print)", "two function values")
checkmessage("print('10' < 10)", "string with number")
checkmessage("print(10 < '23')", "number with string")

-- float->integer conversions
checkmessage("local a = 2.0^100; x = a << 2", "local a")
checkmessage("local a = 1 >> 2.0^100", "has no integer representation")
checkmessage("local a = 10.1 << 2.0^100", "has no integer representation")
checkmessage("local a = 2.0^100 & 1", "has no integer representation")
checkmessage("local a = 2.0^100 & 1e100", "has no integer representation")
checkmessage("local a = 2.0 | 1e40", "has no integer representation")
checkmessage("local a = 2e100 ~ 1", "has no integer representation")
checkmessage("string.sub('a', 2.0^100)", "has no integer representation")
checkmessage("string.rep('a', 3.3)", "has no integer representation")
checkmessage("return 6e40 & 7", "has no integer representation")
checkmessage("return 34 << 7e30", "has no integer representation")
checkmessage("return ~-3e40", "has no integer representation")
checkmessage("return ~-3.009", "has no integer representation")
checkmessage("return 3.009 & 1", "has no integer representation")
checkmessage("return 34 >> {}", "table value")
checkmessage("a = 24 // 0", "divide by zero")
checkmessage("a = 1 % 0", "'n%0'")


-- type error for an object which is neither in an upvalue nor a register.
-- The following code will try to index the value 10 that is stored in
-- the metatable, without moving it to a register.
checkmessage("local a = setmetatable({}, {__index = 10}).x",
             "attempt to index a number value")


-- numeric for loops
checkmessage("for i = {}, 10 do end", "table")
checkmessage("for i = io.stdin, 10 do end", "FILE")
checkmessage("for i = {}, 10 do end", "initial value")
checkmessage("for i = 1, 'x', 10 do end", "string")
checkmessage("for i = 1, {}, 10 do end", "limit")
checkmessage("for i = 1, {} do end", "limit")
checkmessage("for i = 1, 10, print do end", "step")
checkmessage("for i = 1, 10, print do end", "function")

-- passing light userdata instead of full userdata
_G.D = debug
checkmessage([[
  -- create light udata
  local x = D.upvalueid(function () return debug end, 1)
  D.setuservalue(x, {})
]], "light userdata")
_G.D = nil

do   -- named objects (field '__name')
  checkmessage("math.sin(io.input())", "(number expected, got FILE*)")
  _G.XX = setmetatable({}, {__name = "My Type"})
  assert(string.find(tostring(XX), "^My Type"))
  checkmessage("io.input(XX)", "(FILE* expected, got My Type)")
  checkmessage("return XX + 1", "on a My Type value")
  checkmessage("return ~io.stdin", "on a FILE* value")
  checkmessage("return XX < XX", "two My Type values")
  checkmessage("return {} < XX", "table with My Type")
  checkmessage("return XX < io.stdin", "My Type with FILE*")
  _G.XX = nil

  if T then   -- extra tests for 'luaL_tolstring'
    -- bug in 5.4.3; 'luaL_tolstring' with negative indices
    local x = setmetatable({}, {__name="TABLE"})
    assert(T.testC("Ltolstring -1; return 1", x) == tostring(x))

    local a, b = T.testC("pushint 10; Ltolstring -2; return 2", x)
    assert(a == 10 and b == tostring(x))

    setmetatable(x, {__tostring=function (o)
      assert(o == x)
      return "ABC"
    end})
    local a, b, c = T.testC("pushint 10; Ltolstring -2; return 3", x)
    assert(a == x and b == 10 and c == "ABC")
  end
end

-- global functions
checkmessage("(io.write or print){}", "io.write")
checkmessage("(collectgarbage or print){}", "collectgarbage")

-- errors in functions without debug info
do
  local f = function (a) return a + 1 end
  f = assert(load(string.dump(f, true)))
  assert(f(3) == 4)
  checkerr("^%?:%-1:", f, {})

  -- code with a move to a local var ('OP_MOV A B' with A<B)
  f = function () local a; a = {}; return a + 2 end
  -- no debug info (so that 'a' is unknown)
  f = assert(load(string.dump(f, true)))
  -- symbolic execution should not get lost
  checkerr("^%?:%-1:.*table value", f)
end


-- tests for field accesses after RK limit
local t = {}
for i = 1, 1000 do
  t[i] = "a = x" .. i
end
local s = table.concat(t, "; ")
t = nil
checkmessage(s.."; a = bbb + 1", "global 'bbb'")
checkmessage("local _ENV=_ENV;"..s.."; a = bbb + 1", "global 'bbb'")
checkmessage(s.."; local t = {}; a = t.bbb + 1", "field 'bbb'")
checkmessage(s.."; local t = {}; t:bbb()", "method 'bbb'")

checkmessage([[aaa=9
repeat until 3==3
local x=math.sin(math.cos(3))
if math.sin(1) == x then return math.sin(1) end   -- tail call
local a,b = 1, {
  {x='a'..'b'..'c', y='b', z=x},
  {1,2,3,4,5} or 3+3<=3+3,
  3+1>3+1,
  {d = x and aaa[x or y]}}
]], "global 'aaa'")

checkmessage([[
local x,y = {},1
if math.sin(1) == 0 then return 3 end    -- return
x.a()]], "field 'a'")

checkmessage([[
prefix = nil
insert = nil
while 1 do
  local a
  if nil then break end
  insert(prefix, a)
end]], "global 'insert'")

checkmessage([[  -- tail call
  return math.sin("a")
]], "sin")

checkmessage([[collectgarbage("nooption")]], "invalid option")

checkmessage([[x = print .. "a"]], "concatenate")
checkmessage([[x = "a" .. false]], "concatenate")
checkmessage([[x = {} .. 2]], "concatenate")

checkmessage("getmetatable(io.stdin).__gc()", "no value")

checkmessage([[
local Var
local function main()
  NoSuchName (function() Var=0 end)
end
main()
]], "global 'NoSuchName'")
print'+'

a = {}; setmetatable(a, {__index = string})
checkmessage("a:sub()", "bad self")
checkmessage("string.sub('a', {})", "#2")
checkmessage("('a'):sub{}", "#1")

checkmessage("table.sort({1,2,3}, table.sort)", "'table.sort'")
checkmessage("string.gsub('s', 's', setmetatable)", "'setmetatable'")

-- tests for errors in coroutines

local function f (n)
  local c = coroutine.create(f)
  local a,b = coroutine.resume(c)
  return b
end
assert(string.find(f(), "C stack overflow"))

checkmessage("coroutine.yield()", "outside a coroutine")

f = coroutine.wrap(function () table.sort({1,2,3}, coroutine.yield) end)
checkerr("yield across", f)


-- testing size of 'source' info; size of buffer for that info is
-- LUA_IDSIZE, declared as 60 in luaconf. Get one position for '\0'.
idsize = 60 - 1
local function checksize (source)
  -- syntax error
  local _, msg = load("x", source)
  msg = string.match(msg, "^([^:]*):")   -- get source (1st part before ':')
  assert(msg:len() <= idsize)
end

for i = 60 - 10, 60 + 10 do   -- check border cases around 60
  checksize("@" .. string.rep("x", i))   -- file names
  checksize(string.rep("x", i - 10))     -- string sources
  checksize("=" .. string.rep("x", i))   -- exact sources
end


-- testing line error

local function lineerror (s, l)
  local err,msg = pcall(load(s))
  local line = tonumber(string.match(msg, ":(%d+):"))
  assert(line == l or (not line and not l))
end

lineerror("local a\n for i=1,'a' do \n print(i) \n end", 2)
lineerror("\n local a \n for k,v in 3 \n do \n print(k) \n end", 3)
lineerror("\n\n for k,v in \n 3 \n do \n print(k) \n end", 4)
lineerror("function a.x.y ()\na=a+1\nend", 1)

lineerror("a = \na\n+\n{}", 3)
lineerror("a = \n3\n+\n(\n4\n/\nprint)", 6)
lineerror("a = \nprint\n+\n(\n4\n/\n7)", 3)

lineerror("a\n=\n-\n\nprint\n;", 3)

lineerror([[
a
(
23)
]], 1)

lineerror([[
local a = {x = 13}
a
.
x
(
23
)
]], 2)

lineerror([[
local a = {x = 13}
a
.
x
(
23 + a
)
]], 6)

local p = [[
  function g() f() end
  function f(x) error('a', X) end
g()
]]
X=3;lineerror((p), 3)
X=0;lineerror((p), false)
X=1;lineerror((p), 2)
X=2;lineerror((p), 1)


lineerror([[
local b = false
if not b then
  error 'test'
end]], 3)

lineerror([[
local b = false
if not b then
  if not b then
    if not b then
      error 'test'
    end
  end
end]], 5)

do
  -- Force a negative estimate for base line. Error in instruction 2
  -- (after VARARGPREP, GETGLOBAL), with first absolute line information
  -- (forced by too many lines) in instruction 0.
  local s = string.format("%s return __A.x", string.rep("\n", 300))
  lineerror(s, 301)
end


if not _soft then
  -- several tests that exaust the Lua stack
  collectgarbage()
  print"testing stack overflow"
  C = 0
  -- get line where stack overflow will happen
  local l = debug.getinfo(1, "l").currentline + 1
  local function auxy () C=C+1; auxy() end     -- produce a stack overflow
  function y ()
    collectgarbage("stop")   -- avoid running finalizers without stack space
    auxy()
    collectgarbage("restart")
  end

  local function checkstackmessage (m)
    print("(expected stack overflow after " .. C .. " calls)")
    C = 0    -- prepare next count
    return (string.find(m, "stack overflow"))
  end
  -- repeated stack overflows (to check stack recovery)
  assert(checkstackmessage(doit('y()')))
  assert(checkstackmessage(doit('y()')))
  assert(checkstackmessage(doit('y()')))


  -- error lines in stack overflow
  local l1
  local function g(x)
    l1 = debug.getinfo(x, "l").currentline + 2
    collectgarbage("stop")   -- avoid running finalizers without stack space
    auxy()
    collectgarbage("restart")
  end
  local _, stackmsg = xpcall(g, debug.traceback, 1)
  print('+')
  local stack = {}
  for line in string.gmatch(stackmsg, "[^\n]*") do
    local curr = string.match(line, ":(%d+):")
    if curr then table.insert(stack, tonumber(curr)) end
  end
  local i=1
  while stack[i] ~= l1 do
    assert(stack[i] == l)
    i = i+1
  end
  assert(i > 15)


  -- error in error handling
  local res, msg = xpcall(error, error)
  assert(not res and type(msg) == 'string')
  print('+')

  local function f (x)
    if x==0 then error('a\n')
    else
      local aux = function () return f(x-1) end
      local a,b = xpcall(aux, aux)
      return a,b
    end
  end
  f(3)

  local function loop (x,y,z) return 1 + loop(x, y, z) end
 
  local res, msg = xpcall(loop, function (m)
    assert(string.find(m, "stack overflow"))
    checkerr("error handling", loop)
    assert(math.sin(0) == 0)
    return 15
  end)
  assert(msg == 15)

  local f = function ()
    for i = 999900, 1000000, 1 do table.unpack({}, 1, i) end
  end
  checkerr("too many results", f)

end


do
  -- non string messages
  local t = {}
  local res, msg = pcall(function () error(t) end)
  assert(not res and msg == t)

  res, msg = pcall(function () error(nil) end)
  assert(not res and msg == nil)

  local function f() error{msg='x'} end
  res, msg = xpcall(f, function (r) return {msg=r.msg..'y'} end)
  assert(msg.msg == 'xy')

  -- 'assert' with extra arguments
  res, msg = pcall(assert, false, "X", t)
  assert(not res and msg == "X")
 
  -- 'assert' with no message
  res, msg = pcall(function () assert(false) end)
  local line = string.match(msg, "%w+%.lua:(%d+): assertion failed!$")
  assert(tonumber(line) == debug.getinfo(1, "l").currentline - 2)

  -- 'assert' with non-string messages
  res, msg = pcall(assert, false, t)
  assert(not res and msg == t)

  res, msg = pcall(assert, nil, nil)
  assert(not res and msg == nil)

  -- 'assert' without arguments
  res, msg = pcall(assert)
  assert(not res and string.find(msg, "value expected"))
end

-- xpcall with arguments
a, b, c = xpcall(string.find, error, "alo", "al")
assert(a and b == 1 and c == 2)
a, b, c = xpcall(string.find, function (x) return {} end, true, "al")
assert(not a and type(b) == "table" and c == nil)


print("testing tokens in error messages")
checksyntax("syntax error", "", "error", 1)
checksyntax("1.000", "", "1.000", 1)
checksyntax("[[a]]", "", "[[a]]", 1)
checksyntax("'aa'", "", "'aa'", 1)
checksyntax("while << do end", "", "<<", 1)
checksyntax("for >> do end", "", ">>", 1)

-- test invalid non-printable char in a chunk
checksyntax("a\1a = 1", "", "<\\1>", 1)

-- test 255 as first char in a chunk
checksyntax("\255a = 1", "", "<\\255>", 1)

doit('I = load("a=9+"); a=3')
assert(a==3 and not I)
print('+')

lim = 1000
if _soft then lim = 100 end
for i=1,lim do
  doit('a = ')
  doit('a = 4+nil')
end


-- testing syntax limits

local function testrep (init, rep, close, repc, finalresult)
  local s = init .. string.rep(rep, 100) .. close .. string.rep(repc, 100)
  local res, msg = load(s)
  assert(res)   -- 100 levels is OK
  if (finalresult) then
    assert(res() == finalresult)
  end
  s = init .. string.rep(rep, 500)
  local res, msg = load(s)   -- 500 levels not ok
  assert(not res and (string.find(msg, "too many") or
                      string.find(msg, "overflow")))
end

testrep("local a; a", ",a", "= 1", ",1")    -- multiple assignment
testrep("local a; a=", "{", "0", "}")
testrep("return ", "(", "2", ")", 2)
testrep("local function a (x) return x end; return ", "a(", "2.2", ")", 2.2)
testrep("", "do ", "", " end")
testrep("", "while a do ", "", " end")
testrep("local a; ", "if a then else ", "", " end")
testrep("", "function foo () ", "", " end")
testrep("local a = ''; return ", "a..", "'a'", "", "a")
testrep("local a = 1; return ", "a^", "a", "", 1)

checkmessage("a = f(x" .. string.rep(",x", 260) .. ")", "too many registers")


-- testing other limits

-- upvalues
local lim = 127
local  s = "local function fooA ()\n  local "
for j = 1,lim do
  s = s.."a"..j..", "
end
s = s.."b,c\n"
s = s.."local function fooB ()\n  local "
for j = 1,lim do
  s = s.."b"..j..", "
end
s = s.."b\n"
s = s.."function fooC () return b+c"
local c = 1+2
for j = 1,lim do
  s = s.."+a"..j.."+b"..j
  c = c + 2
end
s = s.."\nend  end end"
local a,b = load(s)
assert(c > 255 and string.find(b, "too many upvalues") and
       string.find(b, "line 5"))

-- local variables
s = "\nfunction foo ()\n  local "
for j = 1,300 do
  s = s.."a"..j..", "
end
s = s.."b\n"
local a,b = load(s)
assert(string.find(b, "line 2") and string.find(b, "too many local variables"))

mt.__index = oldmm

print('OK')

-- $Id: testes/events.lua $
-- See Copyright Notice in file all.lua

print('testing metatables')

local debug = require'debug'

X = 20; B = 30

_ENV = setmetatable({}, {__index=_G})

collectgarbage()

X = X+10
assert(X == 30 and _G.X == 20)
B = false
assert(B == false)
_ENV["B"] = undef
assert(B == 30)

assert(getmetatable{} == nil)
assert(getmetatable(4) == nil)
assert(getmetatable(nil) == nil)
a={name = "NAME"}; setmetatable(a, {__metatable = "xuxu",
                    __tostring=function(x) return x.name end})
assert(getmetatable(a) == "xuxu")
assert(tostring(a) == "NAME")
-- cannot change a protected metatable
assert(pcall(setmetatable, a, {}) == false)
a.name = "gororoba"
assert(tostring(a) == "gororoba")

local a, t = {10,20,30; x="10", y="20"}, {}
assert(setmetatable(a,t) == a)
assert(getmetatable(a) == t)
assert(setmetatable(a,nil) == a)
assert(getmetatable(a) == nil)
assert(setmetatable(a,t) == a)


function f (t, i, e)
  assert(not e)
  local p = rawget(t, "parent")
  return (p and p[i]+3), "dummy return"
end

t.__index = f

a.parent = {z=25, x=12, [4] = 24}
assert(a[1] == 10 and a.z == 28 and a[4] == 27 and a.x == "10")

collectgarbage()

a = setmetatable({}, t)
function f(t, i, v) rawset(t, i, v-3) end
setmetatable(t, t)   -- causes a bug in 5.1 !
t.__newindex = f
a[1] = 30; a.x = "101"; a[5] = 200
assert(a[1] == 27 and a.x == 98 and a[5] == 197)

do    -- bug in Lua 5.3.2
  local mt = {}
  mt.__newindex = mt
  local t = setmetatable({}, mt)
  t[1] = 10     -- will segfault on some machines
  assert(mt[1] == 10)
end


local c = {}
a = setmetatable({}, t)
t.__newindex = c
t.__index = c
a[1] = 10; a[2] = 20; a[3] = 90;
for i = 4, 20 do a[i] = i * 10 end
assert(a[1] == 10 and a[2] == 20 and a[3] == 90)
for i = 4, 20 do assert(a[i] == i * 10) end
assert(next(a) == nil)


do
  local a;
  a = setmetatable({}, {__index = setmetatable({},
                     {__index = setmetatable({},
                     {__index = function (_,n) return a[n-3]+4, "lixo" end})})})
  a[0] = 20
  for i=0,10 do
    assert(a[i*3] == 20 + i*4)
  end
end


do  -- newindex
  local foi
  local a = {}
  for i=1,10 do a[i] = 0; a['a'..i] = 0; end
  setmetatable(a, {__newindex = function (t,k,v) foi=true; rawset(t,k,v) end})
  foi = false; a[1]=0; assert(not foi)
  foi = false; a['a1']=0; assert(not foi)
  foi = false; a['a11']=0; assert(foi)
  foi = false; a[11]=0; assert(foi)
  foi = false; a[1]=undef; assert(not foi)
  a[1] = undef
  foi = false; a[1]=nil; assert(foi)
end


setmetatable(t, nil)
function f (t, ...) return t, {...} end
t.__call = f

do
  local x,y = a(table.unpack{'a', 1})
  assert(x==a and y[1]=='a' and y[2]==1 and y[3]==undef)
  x,y = a()
  assert(x==a and y[1]==undef)
end


local b = setmetatable({}, t)
setmetatable(b,t)

function f(op)
  return function (...) cap = {[0] = op, ...} ; return (...) end
end
t.__add = f("add")
t.__sub = f("sub")
t.__mul = f("mul")
t.__div = f("div")
t.__idiv = f("idiv")
t.__mod = f("mod")
t.__unm = f("unm")
t.__pow = f("pow")
t.__len = f("len")
t.__band = f("band")
t.__bor = f("bor")
t.__bxor = f("bxor")
t.__shl = f("shl")
t.__shr = f("shr")
t.__bnot = f("bnot")
t.__lt = f("lt")
t.__le = f("le")


local function checkcap (t)
  assert(#cap + 1 == #t)
  for i = 1, #t do
    assert(cap[i - 1] == t[i])
    assert(math.type(cap[i - 1]) == math.type(t[i]))
  end
end

-- Some tests are done inside small anonymous functions to ensure
-- that constants go to constant table even in debug compilation,
-- when the constant table is very small.
assert(b+5 == b); checkcap{"add", b, 5}
assert(5.2 + b == 5.2); checkcap{"add", 5.2, b}
assert(b+'5' == b); checkcap{"add", b, '5'}
assert(5+b == 5); checkcap{"add", 5, b}
assert('5'+b == '5'); checkcap{"add", '5', b}
b=b-3; assert(getmetatable(b) == t); checkcap{"sub", b, 3}
assert(5-a == 5); checkcap{"sub", 5, a}
assert('5'-a == '5'); checkcap{"sub", '5', a}
assert(a*a == a); checkcap{"mul", a, a}
assert(a/0 == a); checkcap{"div", a, 0}
assert(a/0.0 == a); checkcap{"div", a, 0.0}
assert(a%2 == a); checkcap{"mod", a, 2}
assert(a // (1/0) == a); checkcap{"idiv", a, 1/0}
;(function () assert(a & "hi" == a) end)(); checkcap{"band", a, "hi"}
;(function () assert(10 & a  == 10) end)(); checkcap{"band", 10, a}
;(function () assert(a | 10  == a) end)(); checkcap{"bor", a, 10}
assert(a | "hi" == a); checkcap{"bor", a, "hi"}
assert("hi" ~ a == "hi"); checkcap{"bxor", "hi", a}
;(function () assert(10 ~ a == 10) end)(); checkcap{"bxor", 10, a}
assert(-a == a); checkcap{"unm", a, a}
assert(a^4.0 == a); checkcap{"pow", a, 4.0}
assert(a^'4' == a); checkcap{"pow", a, '4'}
assert(4^a == 4); checkcap{"pow", 4, a}
assert('4'^a == '4'); checkcap{"pow", '4', a}
assert(#a == a); checkcap{"len", a, a}
assert(~a == a); checkcap{"bnot", a, a}
assert(a << 3 == a); checkcap{"shl", a, 3}
assert(1.5 >> a == 1.5); checkcap{"shr", 1.5, a}

-- for comparison operators, all results are true
assert(5.0 > a); checkcap{"lt", a, 5.0}
assert(a >= 10); checkcap{"le", 10, a}
assert(a <= -10.0); checkcap{"le", a, -10.0}
assert(a < -10); checkcap{"lt", a, -10}


-- test for rawlen
t = setmetatable({1,2,3}, {__len = function () return 10 end})
assert(#t == 10 and rawlen(t) == 3)
assert(rawlen"abc" == 3)
assert(not pcall(rawlen, io.stdin))
assert(not pcall(rawlen, 34))
assert(not pcall(rawlen))

-- rawlen for long strings
assert(rawlen(string.rep('a', 1000)) == 1000)


t = {}
t.__lt = function (a,b,c)
  collectgarbage()
  assert(c == nil)
  if type(a) == 'table' then a = a.x end
  if type(b) == 'table' then b = b.x end
 return a<b, "dummy"
end

t.__le = function (a,b,c)
  assert(c == nil)
  if type(a) == 'table' then a = a.x end
  if type(b) == 'table' then b = b.x end
 return a<=b, "dummy"
end

t.__eq = function (a,b,c)
  assert(c == nil)
  if type(a) == 'table' then a = a.x end
  if type(b) == 'table' then b = b.x end
 return a == b, "dummy"
end

function Op(x) return setmetatable({x=x}, t) end

local function test (a, b, c)
  assert(not(Op(1)<Op(1)) and (Op(1)<Op(2)) and not(Op(2)<Op(1)))
  assert(not(1 < Op(1)) and (Op(1) < 2) and not(2 < Op(1)))
  assert(not(Op('a')<Op('a')) and (Op('a')<Op('b')) and not(Op('b')<Op('a')))
  assert(not('a' < Op('a')) and (Op('a') < 'b') and not(Op('b') < Op('a')))
  assert((Op(1)<=Op(1)) and (Op(1)<=Op(2)) and not(Op(2)<=Op(1)))
  assert((Op('a')<=Op('a')) and (Op('a')<=Op('b')) and not(Op('b')<=Op('a')))
  assert(not(Op(1)>Op(1)) and not(Op(1)>Op(2)) and (Op(2)>Op(1)))
  assert(not(Op('a')>Op('a')) and not(Op('a')>Op('b')) and (Op('b')>Op('a')))
  assert((Op(1)>=Op(1)) and not(Op(1)>=Op(2)) and (Op(2)>=Op(1)))
  assert((1 >= Op(1)) and not(1 >= Op(2)) and (Op(2) >= 1))
  assert((Op('a')>=Op('a')) and not(Op('a')>=Op('b')) and (Op('b')>=Op('a')))
  assert(('a' >= Op('a')) and not(Op('a') >= 'b') and (Op('b') >= Op('a')))
  assert(Op(1) == Op(1) and Op(1) ~= Op(2))
  assert(Op('a') == Op('a') and Op('a') ~= Op('b'))
  assert(a == a and a ~= b)
  assert(Op(3) == c)
end

test(Op(1), Op(2), Op(3))


-- test `partial order'

local function rawSet(x)
  local y = {}
  for _,k in pairs(x) do y[k] = 1 end
  return y
end

local function Set(x)
  return setmetatable(rawSet(x), t)
end

t.__lt = function (a,b)
  for k in pairs(a) do
    if not b[k] then return false end
    b[k] = undef
  end
  return next(b) ~= nil
end

t.__le = function (a,b)
  for k in pairs(a) do
    if not b[k] then return false end
  end
  return true
end

assert(Set{1,2,3} < Set{1,2,3,4})
assert(not(Set{1,2,3,4} < Set{1,2,3,4}))
assert((Set{1,2,3,4} <= Set{1,2,3,4}))
assert((Set{1,2,3,4} >= Set{1,2,3,4}))
assert(not (Set{1,3} <= Set{3,5}))
assert(not(Set{1,3} <= Set{3,5}))
assert(not(Set{1,3} >= Set{3,5}))


t.__eq = function (a,b)
  for k in pairs(a) do
    if not b[k] then return false end
    b[k] = undef
  end
  return next(b) == nil
end

local s = Set{1,3,5}
assert(s == Set{3,5,1})
assert(not rawequal(s, Set{3,5,1}))
assert(rawequal(s, s))
assert(Set{1,3,5,1} == rawSet{3,5,1})
assert(rawSet{1,3,5,1} == Set{3,5,1})
assert(Set{1,3,5} ~= Set{3,5,1,6})

-- '__eq' is not used for table accesses
t[Set{1,3,5}] = 1
assert(t[Set{1,3,5}] == undef)


do   -- test invalidating flags
  local mt = {__eq = true}
  local a = setmetatable({10}, mt)
  local b = setmetatable({10}, mt)
  mt.__eq = nil
  assert(a ~= b)   -- no metamethod
  mt.__eq = function (x,y) return x[1] == y[1] end
  assert(a == b)   -- must use metamethod now
end


if not T then
  (Message or print)('\n >>> testC not active: skipping tests for \z
userdata <<<\n')
else
  local u1 = T.newuserdata(0, 1)
  local u2 = T.newuserdata(0, 1)
  local u3 = T.newuserdata(0, 1)
  assert(u1 ~= u2 and u1 ~= u3)
  debug.setuservalue(u1, 1);
  debug.setuservalue(u2, 2);
  debug.setuservalue(u3, 1);
  debug.setmetatable(u1, {__eq = function (a, b)
    return debug.getuservalue(a) == debug.getuservalue(b)
  end})
  debug.setmetatable(u2, {__eq = function (a, b)
    return true
  end})
  assert(u1 == u3 and u3 == u1 and u1 ~= u2)
  assert(u2 == u1 and u2 == u3 and u3 == u2)
  assert(u2 ~= {})   -- different types cannot be equal
  assert(rawequal(u1, u1) and not rawequal(u1, u3))

  local mirror = {}
  debug.setmetatable(u3, {__index = mirror, __newindex = mirror})
  for i = 1, 10 do u3[i] = i end
  for i = 1, 10 do assert(u3[i] == i) end
end


t.__concat = function (a,b,c)
  assert(c == nil)
  if type(a) == 'table' then a = a.val end
  if type(b) == 'table' then b = b.val end
  if A then return a..b
  else
    return setmetatable({val=a..b}, t)
  end
end

c = {val="c"}; setmetatable(c, t)
d = {val="d"}; setmetatable(d, t)

A = true
assert(c..d == 'cd')
assert(0 .."a".."b"..c..d.."e".."f"..(5+3).."g" == "0abcdef8g")

A = false
assert((c..d..c..d).val == 'cdcd')
x = c..d
assert(getmetatable(x) == t and x.val == 'cd')
x = 0 .."a".."b"..c..d.."e".."f".."g"
assert(x.val == "0abcdefg")


-- concat metamethod x numbers (bug in 5.1.1)
c = {}
local x
setmetatable(c, {__concat = function (a,b)
  assert(type(a) == "number" and b == c or type(b) == "number" and a == c)
  return c
end})
assert(c..5 == c and 5 .. c == c)
assert(4 .. c .. 5 == c and 4 .. 5 .. 6 .. 7 .. c == c)


-- test comparison compatibilities
local t1, t2, c, d
t1 = {};  c = {}; setmetatable(c, t1)
d = {}
t1.__eq = function () return true end
t1.__lt = function () return true end
t1.__le = function () return false end
setmetatable(d, t1)
assert(c == d and c < d and not(d <= c))
t2 = {}
t2.__eq = t1.__eq
t2.__lt = t1.__lt
setmetatable(d, t2)
assert(c == d and c < d and not(d <= c))



-- test for several levels of calls
local i
local tt = {
  __call = function (t, ...)
    i = i+1
    if t.f then return t.f(...)
    else return {...}
    end
  end
}

local a = setmetatable({}, tt)
local b = setmetatable({f=a}, tt)
local c = setmetatable({f=b}, tt)

i = 0
x = c(3,4,5)
assert(i == 3 and x[1] == 3 and x[3] == 5)


assert(_G.X == 20)

print'+'

local _g = _G
_ENV = setmetatable({}, {__index=function (_,k) return _g[k] end})


a = {}
rawset(a, "x", 1, 2, 3)
assert(a.x == 1 and rawget(a, "x", 3) == 1)

print '+'

-- testing metatables for basic types
mt = {__index = function (a,b) return a+b end,
      __len = function (x) return math.floor(x) end}
debug.setmetatable(10, mt)
assert(getmetatable(-2) == mt)
assert((10)[3] == 13)
assert((10)["3"] == 13)
assert(#3.45 == 3)
debug.setmetatable(23, nil)
assert(getmetatable(-2) == nil)

debug.setmetatable(true, mt)
assert(getmetatable(false) == mt)
mt.__index = function (a,b) return a or b end
assert((true)[false] == true)
assert((false)[false] == false)
debug.setmetatable(false, nil)
assert(getmetatable(true) == nil)

debug.setmetatable(nil, mt)
assert(getmetatable(nil) == mt)
mt.__add = function (a,b) return (a or 1) + (b or 2) end
assert(10 + nil == 12)
assert(nil + 23 == 24)
assert(nil + nil == 3)
debug.setmetatable(nil, nil)
assert(getmetatable(nil) == nil)

debug.setmetatable(nil, {})


-- loops in delegation
a = {}; setmetatable(a, a); a.__index = a; a.__newindex = a
assert(not pcall(function (a,b) return a[b] end, a, 10))
assert(not pcall(function (a,b,c) a[b] = c end, a, 10, true))

-- bug in 5.1
T, K, V = nil
grandparent = {}
grandparent.__newindex = function(t,k,v) T=t; K=k; V=v end

parent = {}
parent.__newindex = parent
setmetatable(parent, grandparent)

child = setmetatable({}, parent)
child.foo = 10      --> CRASH (on some machines)
assert(T == parent and K == "foo" and V == 10)

print 'OK'

return 12


-- $Id: testes/files.lua $
-- See Copyright Notice in file all.lua

local debug = require "debug"

local maxint = math.maxinteger

assert(type(os.getenv"PATH") == "string")

assert(io.input(io.stdin) == io.stdin)
assert(not pcall(io.input, "non-existent-file"))
assert(io.output(io.stdout) == io.stdout)


local function testerr (msg, f, ...)
  local stat, err = pcall(f, ...)
  return (not stat and string.find(err, msg, 1, true))
end


local function checkerr (msg, f, ...)
  assert(testerr(msg, f, ...))
end


-- cannot close standard files
assert(not io.close(io.stdin) and
       not io.stdout:close() and
       not io.stderr:close())

-- cannot call close method without an argument (new in 5.3.5)
checkerr("got no value", io.stdin.close)


assert(type(io.input()) == "userdata" and io.type(io.output()) == "file")
assert(type(io.stdin) == "userdata" and io.type(io.stderr) == "file")
assert(not io.type(8))
local a = {}; setmetatable(a, {})
assert(not io.type(a))

assert(getmetatable(io.input()).__name == "FILE*")

local a,b,c = io.open('xuxu_nao_existe')
assert(not a and type(b) == "string" and type(c) == "number")

a,b,c = io.open('/a/b/c/d', 'w')
assert(not a and type(b) == "string" and type(c) == "number")

local file = os.tmpname()
local f, msg = io.open(file, "w")
if not f then
  (Message or print)("'os.tmpname' file cannot be open; skipping file tests")

else  --{  most tests here need tmpname
f:close()

print('testing i/o')

local otherfile = os.tmpname()

checkerr("invalid mode", io.open, file, "rw")
checkerr("invalid mode", io.open, file, "rb+")
checkerr("invalid mode", io.open, file, "r+bk")
checkerr("invalid mode", io.open, file, "")
checkerr("invalid mode", io.open, file, "+")
checkerr("invalid mode", io.open, file, "b")
assert(io.open(file, "r+b")):close()
assert(io.open(file, "r+")):close()
assert(io.open(file, "rb")):close()

assert(os.setlocale('C', 'all'))

io.input(io.stdin); io.output(io.stdout);

os.remove(file)
assert(not loadfile(file))
checkerr("", dofile, file)
assert(not io.open(file))
io.output(file)
assert(io.output() ~= io.stdout)

if not _port then   -- invalid seek
  local status, msg, code = io.stdin:seek("set", 1000)
  assert(not status and type(msg) == "string" and type(code) == "number")
end

assert(io.output():seek() == 0)
assert(io.write("alo alo"):seek() == string.len("alo alo"))
assert(io.output():seek("cur", -3) == string.len("alo alo")-3)
assert(io.write("joao"))
assert(io.output():seek("end") == string.len("alo joao"))

assert(io.output():seek("set") == 0)

assert(io.write('"�lo"', "{a}\n", "second line\n", "third line \n"))
assert(io.write('�fourth_line'))
io.output(io.stdout)
collectgarbage()  -- file should be closed by GC
assert(io.input() == io.stdin and rawequal(io.output(), io.stdout))
print('+')

-- test GC for files
collectgarbage()
for i=1,120 do
  for i=1,5 do
    io.input(file)
    assert(io.open(file, 'r'))
    io.lines(file)
  end
  collectgarbage()
end

io.input():close()
io.close()

assert(os.rename(file, otherfile))
assert(not os.rename(file, otherfile))

io.output(io.open(otherfile, "ab"))
assert(io.write("\n\n\t\t  ", 3450, "\n"));
io.close()


do
  -- closing file by scope
  local F = nil
  do
    local f <close> = assert(io.open(file, "w"))
    F = f
  end
  assert(tostring(F) == "file (closed)")
end
assert(os.remove(file))


do
  -- test writing/reading numbers
  local f <close> = assert(io.open(file, "w"))
  f:write(maxint, '\n')
  f:write(string.format("0X%x\n", maxint))
  f:write("0xABCp-3", '\n')
  f:write(0, '\n')
  f:write(-maxint, '\n')
  f:write(string.format("0x%X\n", -maxint))
  f:write("-0xABCp-3", '\n')
  assert(f:close())
  local f <close> = assert(io.open(file, "r"))
  assert(f:read("n") == maxint)
  assert(f:read("n") == maxint)
  assert(f:read("n") == 0xABCp-3)
  assert(f:read("n") == 0)
  assert(f:read("*n") == -maxint)            -- test old format (with '*')
  assert(f:read("n") == -maxint)
  assert(f:read("*n") == -0xABCp-3)            -- test old format (with '*')
end
assert(os.remove(file))


-- testing multiple arguments to io.read
do
  local f <close> = assert(io.open(file, "w"))
  f:write[[
a line
another line
1234
3.45
one
two
three
]]
  local l1, l2, l3, l4, n1, n2, c, dummy
  assert(f:close())
  local f <close> = assert(io.open(file, "r"))
  l1, l2, n1, n2, dummy = f:read("l", "L", "n", "n")
  assert(l1 == "a line" and l2 == "another line\n" and
         n1 == 1234 and n2 == 3.45 and dummy == nil)
  assert(f:close())
  local f <close> = assert(io.open(file, "r"))
  l1, l2, n1, n2, c, l3, l4, dummy = f:read(7, "l", "n", "n", 1, "l", "l")
  assert(l1 == "a line\n" and l2 == "another line" and c == '\n' and
         n1 == 1234 and n2 == 3.45 and l3 == "one" and l4 == "two"
         and dummy == nil)
  assert(f:close())
  local f <close> = assert(io.open(file, "r"))
  -- second item failing
  l1, n1, n2, dummy = f:read("l", "n", "n", "l")
  assert(l1 == "a line" and not n1)
end
assert(os.remove(file))



-- test yielding during 'dofile'
f = assert(io.open(file, "w"))
f:write[[
local x, z = coroutine.yield(10)
local y = coroutine.yield(20)
return x + y * z
]]
assert(f:close())
f = coroutine.wrap(dofile)
assert(f(file) == 10)
assert(f(100, 101) == 20)
assert(f(200) == 100 + 200 * 101)
assert(os.remove(file))


f = assert(io.open(file, "w"))
-- test number termination
f:write[[
-12.3-	-0xffff+  .3|5.E-3X  +234e+13E 0xDEADBEEFDEADBEEFx
0x1.13Ap+3e
]]
-- very long number
f:write("1234"); for i = 1, 1000 do f:write("0") end;  f:write("\n")
-- invalid sequences (must read and discard valid prefixes)
f:write[[
.e+	0.e;	--;  0xX;
]]
assert(f:close())
f = assert(io.open(file, "r"))
assert(f:read("n") == -12.3); assert(f:read(1) == "-")
assert(f:read("n") == -0xffff); assert(f:read(2) == "+ ")
assert(f:read("n") == 0.3); assert(f:read(1) == "|")
assert(f:read("n") == 5e-3); assert(f:read(1) == "X")
assert(f:read("n") == 234e13); assert(f:read(1) == "E")
assert(f:read("n") == 0Xdeadbeefdeadbeef); assert(f:read(2) == "x\n")
assert(f:read("n") == 0x1.13aP3); assert(f:read(1) == "e")

do   -- attempt to read too long number
  assert(not f:read("n"))  -- fails
  local s = f:read("L")   -- read rest of line
  assert(string.find(s, "^00*\n$"))  -- lots of 0's left
end

assert(not f:read("n")); assert(f:read(2) == "e+")
assert(not f:read("n")); assert(f:read(1) == ";")
assert(not f:read("n")); assert(f:read(2) == "-;")
assert(not f:read("n")); assert(f:read(1) == "X")
assert(not f:read("n")); assert(f:read(1) == ";")
assert(not f:read("n")); assert(not f:read(0))   -- end of file
assert(f:close())
assert(os.remove(file))


-- test line generators
assert(not pcall(io.lines, "non-existent-file"))
assert(os.rename(otherfile, file))
io.output(otherfile)
local n = 0
local f = io.lines(file)
while f() do n = n + 1 end;
assert(n == 6)   -- number of lines in the file
checkerr("file is already closed", f)
checkerr("file is already closed", f)
-- copy from file to otherfile
n = 0
for l in io.lines(file) do io.write(l, "\n"); n = n + 1 end
io.close()
assert(n == 6)
-- copy from otherfile back to file
local f = assert(io.open(otherfile))
assert(io.type(f) == "file")
io.output(file)
assert(not io.output():read())
n = 0
for l in f:lines() do io.write(l, "\n"); n = n + 1 end
assert(tostring(f):sub(1, 5) == "file ")
assert(f:close()); io.close()
assert(n == 6)
checkerr("closed file", io.close, f)
assert(tostring(f) == "file (closed)")
assert(io.type(f) == "closed file")
io.input(file)
f = io.open(otherfile):lines()
n = 0
for l in io.lines() do assert(l == f()); n = n + 1 end
f = nil; collectgarbage()
assert(n == 6)
assert(os.remove(otherfile))

do  -- bug in 5.3.1
  io.output(otherfile)
  io.write(string.rep("a", 300), "\n")
  io.close()
  local t ={}; for i = 1, 250 do t[i] = 1 end
  t = {io.lines(otherfile, table.unpack(t))()}
  -- everything ok here
  assert(#t == 250 and t[1] == 'a' and t[#t] == 'a')
  t[#t + 1] = 1    -- one too many
  checkerr("too many arguments", io.lines, otherfile, table.unpack(t))
  collectgarbage()   -- ensure 'otherfile' is closed
  assert(os.remove(otherfile))
end

io.input(file)
do  -- test error returns
  local a,b,c = io.input():write("xuxu")
  assert(not a and type(b) == "string" and type(c) == "number")
end
checkerr("invalid format", io.read, "x")
assert(io.read(0) == "")   -- not eof
assert(io.read(5, 'l') == '"�lo"')
assert(io.read(0) == "")
assert(io.read() == "second line")
local x = io.input():seek()
assert(io.read() == "third line ")
assert(io.input():seek("set", x))
assert(io.read('L') == "third line \n")
assert(io.read(1) == "�")
assert(io.read(string.len"fourth_line") == "fourth_line")
assert(io.input():seek("cur", -string.len"fourth_line"))
assert(io.read() == "fourth_line")
assert(io.read() == "")  -- empty line
assert(io.read('n') == 3450)
assert(io.read(1) == '\n')
assert(not io.read(0))  -- end of file
assert(not io.read(1))  -- end of file
assert(not io.read(30000))  -- end of file
assert(({io.read(1)})[2] == undef)
assert(not io.read())  -- end of file
assert(({io.read()})[2] == undef)
assert(not io.read('n'))  -- end of file
assert(({io.read('n')})[2] == undef)
assert(io.read('a') == '')  -- end of file (OK for 'a')
assert(io.read('a') == '')  -- end of file (OK for 'a')
collectgarbage()
print('+')
io.close(io.input())
checkerr(" input file is closed", io.read)

assert(os.remove(file))

local t = '0123456789'
for i=1,10 do t = t..t; end
assert(string.len(t) == 10*2^10)

io.output(file)
io.write("alo"):write("\n")
io.close()
checkerr(" output file is closed", io.write)
local f = io.open(file, "a+b")
io.output(f)
collectgarbage()

assert(io.write(' ' .. t .. ' '))
assert(io.write(';', 'end of file\n'))
f:flush(); io.flush()
f:close()
print('+')

io.input(file)
assert(io.read() == "alo")
assert(io.read(1) == ' ')
assert(io.read(string.len(t)) == t)
assert(io.read(1) == ' ')
assert(io.read(0))
assert(io.read('a') == ';end of file\n')
assert(not io.read(0))
assert(io.close(io.input()))


-- test errors in read/write
do
  local function ismsg (m)
    -- error message is not a code number
    return (type(m) == "string" and not tonumber(m))
  end

  -- read
  local f = io.open(file, "w")
  local r, m, c = f:read()
  assert(not r and ismsg(m) and type(c) == "number")
  assert(f:close())
  -- write
  f = io.open(file, "r")
  r, m, c = f:write("whatever")
  assert(not r and ismsg(m) and type(c) == "number")
  assert(f:close())
  -- lines
  f = io.open(file, "w")
  r, m = pcall(f:lines())
  assert(r == false and ismsg(m))
  assert(f:close())
end

assert(os.remove(file))

-- test for L format
io.output(file); io.write"\n\nline\nother":close()
io.input(file)
assert(io.read"L" == "\n")
assert(io.read"L" == "\n")
assert(io.read"L" == "line\n")
assert(io.read"L" == "other")
assert(not io.read"L")
io.input():close()

local f = assert(io.open(file))
local s = ""
for l in f:lines("L") do s = s .. l end
assert(s == "\n\nline\nother")
f:close()

io.input(file)
s = ""
for l in io.lines(nil, "L") do s = s .. l end
assert(s == "\n\nline\nother")
io.input():close()

s = ""
for l in io.lines(file, "L") do s = s .. l end
assert(s == "\n\nline\nother")

s = ""
for l in io.lines(file, "l") do s = s .. l end
assert(s == "lineother")

io.output(file); io.write"a = 10 + 34\na = 2*a\na = -a\n":close()
local t = {}
assert(load(io.lines(file, "L"), nil, nil, t))()
assert(t.a == -((10 + 34) * 2))


do   -- testing closing file in line iteration

  -- get the to-be-closed variable from a loop
  local function gettoclose (lv)
    lv = lv + 1
    local stvar = 0   -- to-be-closed is 4th state variable in the loop
    for i = 1, 1000 do
      local n, v = debug.getlocal(lv, i)
      if n == "(for state)" then
        stvar = stvar + 1
        if stvar == 4 then return v end
      end
    end
  end

  local f
  for l in io.lines(file) do
    f = gettoclose(1)
    assert(io.type(f) == "file")
    break
  end
  assert(io.type(f) == "closed file")

  f = nil
  local function foo (name)
    for l in io.lines(name) do
      f = gettoclose(1)
      assert(io.type(f) == "file")
      error(f)   -- exit loop with an error
    end
  end
  local st, msg = pcall(foo, file)
  assert(st == false and io.type(msg) == "closed file")

end


-- test for multipe arguments in 'lines'
io.output(file); io.write"0123456789\n":close()
for a,b in io.lines(file, 1, 1) do
  if a == "\n" then assert(not b)
  else assert(tonumber(a) == tonumber(b) - 1)
  end
end

for a,b,c in io.lines(file, 1, 2, "a") do
  assert(a == "0" and b == "12" and c == "3456789\n")
end

for a,b,c in io.lines(file, "a", 0, 1) do
  if a == "" then break end
  assert(a == "0123456789\n" and not b and not c)
end
collectgarbage()   -- to close file in previous iteration

io.output(file); io.write"00\n10\n20\n30\n40\n":close()
for a, b in io.lines(file, "n", "n") do
  if a == 40 then assert(not b)
  else assert(a == b - 10)
  end
end


-- test load x lines
io.output(file);
io.write[[
local y
= X
X =
X *
2 +
X;
X =
X
-                                   y;
]]:close()
_G.X = 1
assert(not load((io.lines(file))))
collectgarbage()   -- to close file in previous iteration
load((io.lines(file, "L")))()
assert(_G.X == 2)
load((io.lines(file, 1)))()
assert(_G.X == 4)
load((io.lines(file, 3)))()
assert(_G.X == 8)

print('+')

local x1 = "string\n\n\\com \"\"''coisas [[estranhas]] ]]'"
io.output(file)
assert(io.write(string.format("x2 = %q\n-- comment without ending EOS", x1)))
io.close()
assert(loadfile(file))()
assert(x1 == x2)
print('+')
assert(os.remove(file))
assert(not os.remove(file))
assert(not os.remove(otherfile))

-- testing loadfile
local function testloadfile (s, expres)
  io.output(file)
  if s then io.write(s) end
  io.close()
  local res = assert(loadfile(file))()
  assert(os.remove(file))
  assert(res == expres)
end

-- loading empty file
testloadfile(nil, nil)

-- loading file with initial comment without end of line
testloadfile("# a non-ending comment", nil)


-- checking Unicode BOM in files
testloadfile("\xEF\xBB\xBF# some comment\nreturn 234", 234)
testloadfile("\xEF\xBB\xBFreturn 239", 239)
testloadfile("\xEF\xBB\xBF", nil)   -- empty file with a BOM


-- checking line numbers in files with initial comments
testloadfile("# a comment\nreturn require'debug'.getinfo(1).currentline", 2)


-- loading binary file
io.output(io.open(file, "wb"))
assert(io.write(string.dump(function () return 10, '\0alo\255', 'hi' end)))
io.close()
a, b, c = assert(loadfile(file))()
assert(a == 10 and b == "\0alo\255" and c == "hi")
assert(os.remove(file))

-- bug in 5.2.1
do
  io.output(io.open(file, "wb"))
  -- save function with no upvalues
  assert(io.write(string.dump(function () return 1 end)))
  io.close()
  f = assert(loadfile(file, "b", {}))
  assert(type(f) == "function" and f() == 1)
  assert(os.remove(file))
end

-- loading binary file with initial comment
io.output(io.open(file, "wb"))
assert(io.write("#this is a comment for a binary file\0\n",
                string.dump(function () return 20, '\0\0\0' end)))
io.close()
a, b, c = assert(loadfile(file))()
assert(a == 20 and b == "\0\0\0" and c == nil)
assert(os.remove(file))


-- 'loadfile' with 'env'
do
  local f = io.open(file, 'w')
  f:write[[
    if (...) then a = 15; return b, c, d
    else return _ENV
    end
  ]]
  f:close()
  local t = {b = 12, c = "xuxu", d = print}
  local f = assert(loadfile(file, 't', t))
  local b, c, d = f(1)
  assert(t.a == 15 and b == 12 and c == t.c and d == print)
  assert(f() == t)
  f = assert(loadfile(file, 't', nil))
  assert(f() == nil)
  f = assert(loadfile(file))
  assert(f() == _G)
  assert(os.remove(file))
end


-- 'loadfile' x modes
do
  io.open(file, 'w'):write("return 10"):close()
  local s, m = loadfile(file, 'b')
  assert(not s and string.find(m, "a text chunk"))
  io.open(file, 'w'):write("\27 return 10"):close()
  local s, m = loadfile(file, 't')
  assert(not s and string.find(m, "a binary chunk"))
  assert(os.remove(file))
end


io.output(file)
assert(io.write("qualquer coisa\n"))
assert(io.write("mais qualquer coisa"))
io.close()
assert(io.output(assert(io.open(otherfile, 'wb')))
       :write("outra coisa\0\1\3\0\0\0\0\255\0")
       :close())

local filehandle = assert(io.open(file, 'r+'))
local otherfilehandle = assert(io.open(otherfile, 'rb'))
assert(filehandle ~= otherfilehandle)
assert(type(filehandle) == "userdata")
assert(filehandle:read('l') == "qualquer coisa")
io.input(otherfilehandle)
assert(io.read(string.len"outra coisa") == "outra coisa")
assert(filehandle:read('l') == "mais qualquer coisa")
filehandle:close();
assert(type(filehandle) == "userdata")
io.input(otherfilehandle)
assert(io.read(4) == "\0\1\3\0")
assert(io.read(3) == "\0\0\0")
assert(io.read(0) == "")        -- 255 is not eof
assert(io.read(1) == "\255")
assert(io.read('a') == "\0")
assert(not io.read(0))
assert(otherfilehandle == io.input())
otherfilehandle:close()
assert(os.remove(file))
assert(os.remove(otherfile))
collectgarbage()

io.output(file)
  :write[[
 123.4	-56e-2  not a number
second line
third line

and the rest of the file
]]
  :close()
io.input(file)
local _,a,b,c,d,e,h,__ = io.read(1, 'n', 'n', 'l', 'l', 'l', 'a', 10)
assert(io.close(io.input()))
assert(_ == ' ' and not __)
assert(type(a) == 'number' and a==123.4 and b==-56e-2)
assert(d=='second line' and e=='third line')
assert(h==[[

and the rest of the file
]])
assert(os.remove(file))
collectgarbage()

-- testing buffers
do
  local f = assert(io.open(file, "w"))
  local fr = assert(io.open(file, "r"))
  assert(f:setvbuf("full", 2000))
  f:write("x")
  assert(fr:read("all") == "")  -- full buffer; output not written yet
  f:close()
  fr:seek("set")
  assert(fr:read("all") == "x")   -- `close' flushes it
  f = assert(io.open(file), "w")
  assert(f:setvbuf("no"))
  f:write("x")
  fr:seek("set")
  assert(fr:read("all") == "x")  -- no buffer; output is ready
  f:close()
  f = assert(io.open(file, "a"))
  assert(f:setvbuf("line"))
  f:write("x")
  fr:seek("set", 1)
  assert(fr:read("all") == "")   -- line buffer; no output without `\n'
  f:write("a\n"):seek("set", 1)
  assert(fr:read("all") == "xa\n")  -- now we have a whole line
  f:close(); fr:close()
  assert(os.remove(file))
end


if not _soft then
  print("testing large files (> BUFSIZ)")
  io.output(file)
  for i=1,5001 do io.write('0123456789123') end
  io.write('\n12346'):close()
  io.input(file)
  local x = io.read('a')
  io.input():seek('set', 0)
  local y = io.read(30001)..io.read(1005)..io.read(0)..
            io.read(1)..io.read(100003)
  assert(x == y and string.len(x) == 5001*13 + 6)
  io.input():seek('set', 0)
  y = io.read()  -- huge line
  assert(x == y..'\n'..io.read())
  assert(not io.read())
  io.close(io.input())
  assert(os.remove(file))
  x = nil; y = nil
end

if not _port then
  local progname
  do  -- get name of running executable
    local arg = arg or ARG
    local i = 0
    while arg[i] do i = i - 1 end
    progname = '"' .. arg[i + 1] .. '"'
  end
  print("testing popen/pclose and execute")
  -- invalid mode for popen
  checkerr("invalid mode", io.popen, "cat", "")
  checkerr("invalid mode", io.popen, "cat", "r+")
  checkerr("invalid mode", io.popen, "cat", "rw")
  do  -- basic tests for popen
    local file = os.tmpname()
    local f = assert(io.popen("cat - > " .. file, "w"))
    f:write("a line")
    assert(f:close())
    local f = assert(io.popen("cat - < " .. file, "r"))
    assert(f:read("a") == "a line")
    assert(f:close())
    assert(os.remove(file))
  end

  local tests = {
    -- command,   what,  code
    {"ls > /dev/null", "ok"},
    {"not-to-be-found-command", "exit"},
    {"exit 3", "exit", 3},
    {"exit 129", "exit", 129},
    {"kill -s HUP $$", "signal", 1},
    {"kill -s KILL $$", "signal", 9},
    {"sh -c 'kill -s HUP $$'", "exit"},
    {progname .. ' -e " "', "ok"},
    {progname .. ' -e "os.exit(0, true)"', "ok"},
    {progname .. ' -e "os.exit(20, true)"', "exit", 20},
  }
  print("\n(some error messages are expected now)")
  for _, v in ipairs(tests) do
    local x, y, z = io.popen(v[1]):close()
    local x1, y1, z1 = os.execute(v[1])
    assert(x == x1 and y == y1 and z == z1)
    if v[2] == "ok" then
      assert(x and y == 'exit' and z == 0)
    else
      assert(not x and y == v[2])   -- correct status and 'what'
      -- correct code if known (but always different from 0)
      assert((v[3] == nil and z > 0) or v[3] == z)
    end
  end
end


-- testing tmpfile
f = io.tmpfile()
assert(io.type(f) == "file")
f:write("alo")
f:seek("set")
assert(f:read"a" == "alo")

end --}

print'+'

print("testing date/time")

assert(os.date("") == "")
assert(os.date("!") == "")
assert(os.date("\0\0") == "\0\0")
assert(os.date("!\0\0") == "\0\0")
local x = string.rep("a", 10000)
assert(os.date(x) == x)
local t = os.time()
D = os.date("*t", t)
assert(os.date(string.rep("%d", 1000), t) ==
       string.rep(os.date("%d", t), 1000))
assert(os.date(string.rep("%", 200)) == string.rep("%", 100))

local function checkDateTable (t)
  _G.D = os.date("*t", t)
  assert(os.time(D) == t)
  load(os.date([[assert(D.year==%Y and D.month==%m and D.day==%d and
    D.hour==%H and D.min==%M and D.sec==%S and
    D.wday==%w+1 and D.yday==%j)]], t))()
  _G.D = nil
end

checkDateTable(os.time())
if not _port then
  -- assume that time_t can represent these values
  checkDateTable(0)
  checkDateTable(1)
  checkDateTable(1000)
  checkDateTable(0x7fffffff)
  checkDateTable(0x80000000)
end

checkerr("invalid conversion specifier", os.date, "%")
checkerr("invalid conversion specifier", os.date, "%9")
checkerr("invalid conversion specifier", os.date, "%")
checkerr("invalid conversion specifier", os.date, "%O")
checkerr("invalid conversion specifier", os.date, "%E")
checkerr("invalid conversion specifier", os.date, "%Ea")

checkerr("not an integer", os.time, {year=1000, month=1, day=1, hour='x'})
checkerr("not an integer", os.time, {year=1000, month=1, day=1, hour=1.5})

checkerr("missing", os.time, {hour = 12})   -- missing date


if string.packsize("i") == 4 then   -- 4-byte ints
  checkerr("field 'year' is out-of-bound", os.time,
              {year = -(1 << 31) + 1899, month = 1, day = 1})
end

if not _port then
  -- test Posix-specific modifiers
  assert(type(os.date("%Ex")) == 'string')
  assert(type(os.date("%Oy")) == 'string')

  -- test large dates (assume at least 4-byte ints and time_t)
  local t0 = os.time{year = 1970, month = 1, day = 0}
  local t1 = os.time{year = 1970, month = 1, day = 0, sec = (1 << 31) - 1}
  assert(t1 - t0 == (1 << 31) - 1)
  t0 = os.time{year = 1970, month = 1, day = 1}
  t1 = os.time{year = 1970, month = 1, day = 1, sec = -(1 << 31)}
  assert(t1 - t0 == -(1 << 31))

  -- test out-of-range dates (at least for Unix)
  if maxint >= 2^62 then  -- cannot do these tests in Small Lua
    -- no arith overflows
    checkerr("out-of-bound", os.time, {year = -maxint, month = 1, day = 1})
    if string.packsize("i") == 4 then   -- 4-byte ints
      if testerr("out-of-bound", os.date, "%Y", 2^40) then
        -- time_t has 4 bytes and therefore cannot represent year 4000
        print("  4-byte time_t")
        checkerr("cannot be represented", os.time, {year=4000, month=1, day=1})
      else
        -- time_t has 8 bytes; an int year cannot represent a huge time
        print("  8-byte time_t")
        checkerr("cannot be represented", os.date, "%Y", 2^60)

        -- this is the maximum year
        assert(tonumber(os.time
          {year=(1 << 31) + 1899, month=12, day=31, hour=23, min=59, sec=59}))

        -- this is too much
        checkerr("represented", os.time,
          {year=(1 << 31) + 1899, month=12, day=31, hour=23, min=59, sec=60})
      end

      -- internal 'int' fields cannot hold these values
      checkerr("field 'day' is out-of-bound", os.time,
                  {year = 0, month = 1, day = 2^32})

      checkerr("field 'month' is out-of-bound", os.time,
                  {year = 0, month = -((1 << 31) + 1), day = 1})

      checkerr("field 'year' is out-of-bound", os.time,
                  {year = (1 << 31) + 1900, month = 1, day = 1})

    else    -- 8-byte ints
      -- assume time_t has 8 bytes too
      print("  8-byte time_t")
      assert(tonumber(os.date("%Y", 2^60)))

      -- but still cannot represent a huge year
      checkerr("cannot be represented", os.time, {year=2^60, month=1, day=1})
    end
  end
end

do
  local D = os.date("*t")
  local t = os.time(D)
  if D.isdst == nil then
    print("no daylight saving information")
  else
    assert(type(D.isdst) == 'boolean')
  end
  D.isdst = nil
  local t1 = os.time(D)
  assert(t == t1)   -- if isdst is absent uses correct default
end

local D = os.date("*t")
t = os.time(D)
D.year = D.year-1;
local t1 = os.time(D)
-- allow for leap years
assert(math.abs(os.difftime(t,t1)/(24*3600) - 365) < 2)

-- should not take more than 1 second to execute these two lines
t = os.time()
t1 = os.time(os.date("*t"))
local diff = os.difftime(t1,t)
assert(0 <= diff and diff <= 1)
diff = os.difftime(t,t1)
assert(-1 <= diff and diff <= 0)

local t1 = os.time{year=2000, month=10, day=1, hour=23, min=12}
local t2 = os.time{year=2000, month=10, day=1, hour=23, min=10, sec=19}
assert(os.difftime(t1,t2) == 60*2-19)

-- since 5.3.3, 'os.time' normalizes table fields
t1 = {year = 2005, month = 1, day = 1, hour = 1, min = 0, sec = -3602}
os.time(t1)
assert(t1.day == 31 and t1.month == 12 and t1.year == 2004 and
       t1.hour == 23 and t1.min == 59 and t1.sec == 58 and
       t1.yday == 366)

io.output(io.stdout)
local t = os.date('%d %m %Y %H %M %S')
local d, m, a, h, min, s = string.match(t,
                             "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
d = tonumber(d)
m = tonumber(m)
a = tonumber(a)
h = tonumber(h)
min = tonumber(min)
s = tonumber(s)
io.write(string.format('test done on %2.2d/%2.2d/%d', d, m, a))
io.write(string.format(', at %2.2d:%2.2d:%2.2d\n', h, min, s))
io.write(string.format('%s\n', _VERSION))


-- $Id: testes/gc.lua $
-- See Copyright Notice in file all.lua

print('testing incremental garbage collection')

local debug = require"debug"

assert(collectgarbage("isrunning"))

collectgarbage()

local oldmode = collectgarbage("incremental")

-- changing modes should return previous mode
assert(collectgarbage("generational") == "incremental")
assert(collectgarbage("generational") == "generational")
assert(collectgarbage("incremental") == "generational")
assert(collectgarbage("incremental") == "incremental")


local function nop () end

local function gcinfo ()
  return collectgarbage"count" * 1024
end


-- test weird parameters to 'collectgarbage'
do
  -- save original parameters
  local a = collectgarbage("setpause", 200)
  local b = collectgarbage("setstepmul", 200)
  local t = {0, 2, 10, 90, 500, 5000, 30000, 0x7ffffffe}
  for i = 1, #t do
    local p = t[i]
    for j = 1, #t do
      local m = t[j]
      collectgarbage("setpause", p)
      collectgarbage("setstepmul", m)
      collectgarbage("step", 0)
      collectgarbage("step", 10000)
    end
  end
  -- restore original parameters
  collectgarbage("setpause", a)
  collectgarbage("setstepmul", b)
  collectgarbage()
end


_G["while"] = 234


--
-- tests for GC activation when creating different kinds of objects
--
local function GC1 ()
  local u
  local b     -- (above 'u' it in the stack)
  local finish = false
  u = setmetatable({}, {__gc = function () finish = true end})
  b = {34}
  repeat u = {} until finish
  assert(b[1] == 34)   -- 'u' was collected, but 'b' was not

  finish = false; local i = 1
  u = setmetatable({}, {__gc = function () finish = true end})
  repeat i = i + 1; u = tostring(i) .. tostring(i) until finish
  assert(b[1] == 34)   -- 'u' was collected, but 'b' was not

  finish = false
  u = setmetatable({}, {__gc = function () finish = true end})
  repeat local i; u = function () return i end until finish
  assert(b[1] == 34)   -- 'u' was collected, but 'b' was not
end

local function GC2 ()
  local u
  local finish = false
  u = {setmetatable({}, {__gc = function () finish = true end})}
  local b = {34}
  repeat u = {{}} until finish
  assert(b[1] == 34)   -- 'u' was collected, but 'b' was not

  finish = false; local i = 1
  u = {setmetatable({}, {__gc = function () finish = true end})}
  repeat i = i + 1; u = {tostring(i) .. tostring(i)} until finish
  assert(b[1] == 34)   -- 'u' was collected, but 'b' was not

  finish = false
  u = {setmetatable({}, {__gc = function () finish = true end})}
  repeat local i; u = {function () return i end} until finish
  assert(b[1] == 34)   -- 'u' was collected, but 'b' was not
end

local function GC()  GC1(); GC2() end


do
  print("creating many objects")

  local limit = 5000

  for i = 1, limit do
    local a = {}; a = nil
  end

  local a = "a"

  for i = 1, limit do
    a = i .. "b";
    a = string.gsub(a, '(%d%d*)', "%1 %1")
    a = "a"
  end



  a = {}

  function a:test ()
    for i = 1, limit do
      load(string.format("function temp(a) return 'a%d' end", i), "")()
      assert(temp() == string.format('a%d', i))
    end
  end

  a:test()

end


-- collection of functions without locals, globals, etc.
do local f = function () end end


print("functions with errors")
prog = [[
do
  a = 10;
  function foo(x,y)
    a = sin(a+0.456-0.23e-12);
    return function (z) return sin(%x+z) end
  end
  local x = function (w) a=a+w; end
end
]]
do
  local step = 1
  if _soft then step = 13 end
  for i=1, string.len(prog), step do
    for j=i, string.len(prog), step do
      pcall(load(string.sub(prog, i, j), ""))
    end
  end
end

foo = nil
print('long strings')
x = "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
assert(string.len(x)==80)
s = ''
k = math.min(300, (math.maxinteger // 80) // 2)
for n = 1, k do s = s..x; j=tostring(n)  end
assert(string.len(s) == k*80)
s = string.sub(s, 1, 10000)
s, i = string.gsub(s, '(%d%d%d%d)', '')
assert(i==10000 // 4)
s = nil
x = nil

assert(_G["while"] == 234)


--
-- test the "size" of basic GC steps (whatever they mean...)
--
do
print("steps")

  print("steps (2)")

  local function dosteps (siz)
    collectgarbage()
    local a = {}
    for i=1,100 do a[i] = {{}}; local b = {} end
    local x = gcinfo()
    local i = 0
    repeat   -- do steps until it completes a collection cycle
      i = i+1
    until collectgarbage("step", siz)
    assert(gcinfo() < x)
    return i    -- number of steps
  end

  collectgarbage"stop"

  if not _port then
    assert(dosteps(10) < dosteps(2))
  end

  -- collector should do a full collection with so many steps
  assert(dosteps(20000) == 1)
  assert(collectgarbage("step", 20000) == true)
  assert(collectgarbage("step", 20000) == true)

  assert(not collectgarbage("isrunning"))
  collectgarbage"restart"
  assert(collectgarbage("isrunning"))

end


if not _port then
  -- test the pace of the collector
  collectgarbage(); collectgarbage()
  local x = gcinfo()
  collectgarbage"stop"
  repeat
    local a = {}
  until gcinfo() > 3 * x
  collectgarbage"restart"
  assert(collectgarbage("isrunning"))
  repeat
    local a = {}
  until gcinfo() <= x * 2
end


print("clearing tables")
lim = 15
a = {}
-- fill a with `collectable' indices
for i=1,lim do a[{}] = i end
b = {}
for k,v in pairs(a) do b[k]=v end
-- remove all indices and collect them
for n in pairs(b) do
  a[n] = undef
  assert(type(n) == 'table' and next(n) == nil)
  collectgarbage()
end
b = nil
collectgarbage()
for n in pairs(a) do error'cannot be here' end
for i=1,lim do a[i] = i end
for i=1,lim do assert(a[i] == i) end


print('weak tables')
a = {}; setmetatable(a, {__mode = 'k'});
-- fill a with some `collectable' indices
for i=1,lim do a[{}] = i end
-- and some non-collectable ones
for i=1,lim do a[i] = i end
for i=1,lim do local s=string.rep('@', i); a[s] = s..'#' end
collectgarbage()
local i = 0
for k,v in pairs(a) do assert(k==v or k..'#'==v); i=i+1 end
assert(i == 2*lim)

a = {}; setmetatable(a, {__mode = 'v'});
a[1] = string.rep('b', 21)
collectgarbage()
assert(a[1])   -- strings are *values*
a[1] = undef
-- fill a with some `collectable' values (in both parts of the table)
for i=1,lim do a[i] = {} end
for i=1,lim do a[i..'x'] = {} end
-- and some non-collectable ones
for i=1,lim do local t={}; a[t]=t end
for i=1,lim do a[i+lim]=i..'x' end
collectgarbage()
local i = 0
for k,v in pairs(a) do assert(k==v or k-lim..'x' == v); i=i+1 end
assert(i == 2*lim)

a = {}; setmetatable(a, {__mode = 'kv'});
local x, y, z = {}, {}, {}
-- keep only some items
a[1], a[2], a[3] = x, y, z
a[string.rep('$', 11)] = string.rep('$', 11)
-- fill a with some `collectable' values
for i=4,lim do a[i] = {} end
for i=1,lim do a[{}] = i end
for i=1,lim do local t={}; a[t]=t end
collectgarbage()
assert(next(a) ~= nil)
local i = 0
for k,v in pairs(a) do
  assert((k == 1 and v == x) or
         (k == 2 and v == y) or
         (k == 3 and v == z) or k==v);
  i = i+1
end
assert(i == 4)
x,y,z=nil
collectgarbage()
assert(next(a) == string.rep('$', 11))


-- 'bug' in 5.1
a = {}
local t = {x = 10}
local C = setmetatable({key = t}, {__mode = 'v'})
local C1 = setmetatable({[t] = 1}, {__mode = 'k'})
a.x = t  -- this should not prevent 't' from being removed from
         -- weak table 'C' by the time 'a' is finalized

setmetatable(a, {__gc = function (u)
                          assert(C.key == nil)
                          assert(type(next(C1)) == 'table')
                          end})

a, t = nil
collectgarbage()
collectgarbage()
assert(next(C) == nil and next(C1) == nil)
C, C1 = nil


-- ephemerons
local mt = {__mode = 'k'}
a = {{10},{20},{30},{40}}; setmetatable(a, mt)
x = nil
for i = 1, 100 do local n = {}; a[n] = {k = {x}}; x = n end
GC()
local n = x
local i = 0
while n do n = a[n].k[1]; i = i + 1 end
assert(i == 100)
x = nil
GC()
for i = 1, 4 do assert(a[i][1] == i * 10); a[i] = undef end
assert(next(a) == nil)

local K = {}
a[K] = {}
for i=1,10 do a[K][i] = {}; a[a[K][i]] = setmetatable({}, mt) end
x = nil
local k = 1
for j = 1,100 do
  local n = {}; local nk = k%10 + 1
  a[a[K][nk]][n] = {x, k = k}; x = n; k = nk
end
GC()
local n = x
local i = 0
while n do local t = a[a[K][k]][n]; n = t[1]; k = t.k; i = i + 1 end
assert(i == 100)
K = nil
GC()
-- assert(next(a) == nil)


-- testing errors during GC
if T then
  collectgarbage("stop")   -- stop collection
  local u = {}
  local s = {}; setmetatable(s, {__mode = 'k'})
  setmetatable(u, {__gc = function (o)
    local i = s[o]
    s[i] = true
    assert(not s[i - 1])   -- check proper finalization order
    if i == 8 then error("@expected@") end   -- error during GC
  end})

  for i = 6, 10 do
    local n = setmetatable({}, getmetatable(u))
    s[n] = i
  end

  warn("@on"); warn("@store")
  collectgarbage()
  assert(string.find(_WARN, "error in __gc metamethod"))
  assert(string.match(_WARN, "@(.-)@") == "expected"); _WARN = false
  for i = 8, 10 do assert(s[i]) end

  for i = 1, 5 do
    local n = setmetatable({}, getmetatable(u))
    s[n] = i
  end

  collectgarbage()
  for i = 1, 10 do assert(s[i]) end

  getmetatable(u).__gc = nil
  warn("@normal")

end
print '+'


-- testing userdata
if T==nil then
  (Message or print)('\n >>> testC not active: skipping userdata GC tests <<<\n')

else

  local function newproxy(u)
    return debug.setmetatable(T.newuserdata(0), debug.getmetatable(u))
  end

  collectgarbage("stop")   -- stop collection
  local u = newproxy(nil)
  debug.setmetatable(u, {__gc = true})
  local s = 0
  local a = {[u] = 0}; setmetatable(a, {__mode = 'vk'})
  for i=1,10 do a[newproxy(u)] = i end
  for k in pairs(a) do assert(getmetatable(k) == getmetatable(u)) end
  local a1 = {}; for k,v in pairs(a) do a1[k] = v end
  for k,v in pairs(a1) do a[v] = k end
  for i =1,10 do assert(a[i]) end
  getmetatable(u).a = a1
  getmetatable(u).u = u
  do
    local u = u
    getmetatable(u).__gc = function (o)
      assert(a[o] == 10-s)
      assert(a[10-s] == undef) -- udata already removed from weak table
      assert(getmetatable(o) == getmetatable(u))
    assert(getmetatable(o).a[o] == 10-s)
      s=s+1
    end
  end
  a1, u = nil
  assert(next(a) ~= nil)
  collectgarbage()
  assert(s==11)
  collectgarbage()
  assert(next(a) == nil)  -- finalized keys are removed in two cycles
end


-- __gc x weak tables
local u = setmetatable({}, {__gc = true})
-- __gc metamethod should be collected before running
setmetatable(getmetatable(u), {__mode = "v"})
getmetatable(u).__gc = function (o) os.exit(1) end  -- cannot happen
u = nil
collectgarbage()

local u = setmetatable({}, {__gc = true})
local m = getmetatable(u)
m.x = {[{0}] = 1; [0] = {1}}; setmetatable(m.x, {__mode = "kv"});
m.__gc = function (o)
  assert(next(getmetatable(o).x) == nil)
  m = 10
end
u, m = nil
collectgarbage()
assert(m==10)

do   -- tests for string keys in weak tables
  collectgarbage(); collectgarbage()
  local m = collectgarbage("count")         -- current memory
  local a = setmetatable({}, {__mode = "kv"})
  a[string.rep("a", 2^22)] = 25   -- long string key -> number value
  a[string.rep("b", 2^22)] = {}   -- long string key -> colectable value
  a[{}] = 14                     -- colectable key
  assert(collectgarbage("count") > m + 2^13)    -- 2^13 == 2 * 2^22 in KB
  collectgarbage()
  assert(collectgarbage("count") >= m + 2^12 and
        collectgarbage("count") < m + 2^13)    -- one key was collected
  local k, v = next(a)   -- string key with number value preserved
  assert(k == string.rep("a", 2^22) and v == 25)
  assert(next(a, k) == nil)  -- everything else cleared
  assert(a[string.rep("b", 2^22)] == undef)
  a[k] = undef        -- erase this last entry
  k = nil
  collectgarbage()
  assert(next(a) == nil)
  -- make sure will not try to compare with dead key
  assert(a[string.rep("b", 100)] == undef)
  assert(collectgarbage("count") <= m + 1)   -- eveything collected
end


-- errors during collection
if T then
  warn("@store")
  u = setmetatable({}, {__gc = function () error "@expected error" end})
  u = nil
  collectgarbage()
  assert(string.find(_WARN, "@expected error")); _WARN = false
  warn("@normal")
end


if not _soft then
  print("long list")
  local a = {}
  for i = 1,200000 do
    a = {next = a}
  end
  a = nil
  collectgarbage()
end

-- create many threads with self-references and open upvalues
print("self-referenced threads")
local thread_id = 0
local threads = {}

local function fn (thread)
    local x = {}
    threads[thread_id] = function()
                             thread = x
                         end
    coroutine.yield()
end

while thread_id < 1000 do
    local thread = coroutine.create(fn)
    coroutine.resume(thread, thread)
    thread_id = thread_id + 1
end


-- Create a closure (function inside 'f') with an upvalue ('param') that
-- points (through a table) to the closure itself and to the thread
-- ('co' and the initial value of 'param') where closure is running.
-- Then, assert that table (and therefore everything else) will be
-- collected.
do
  local collected = false   -- to detect collection
  collectgarbage(); collectgarbage("stop")
  do
    local function f (param)
      ;(function ()
        assert(type(f) == 'function' and type(param) == 'thread')
        param = {param, f}
        setmetatable(param, {__gc = function () collected = true end})
        coroutine.yield(100)
      end)()
    end
    local co = coroutine.create(f)
    assert(coroutine.resume(co, co))
  end
  -- Now, thread and closure are not reacheable any more.
  collectgarbage()
  assert(collected)
  collectgarbage("restart")
end


do
  collectgarbage()
  collectgarbage"stop"
  collectgarbage("step", 0)   -- steps should not unblock the collector
  local x = gcinfo()
  repeat
    for i=1,1000 do _ENV.a = {} end   -- no collection during the loop
  until gcinfo() > 2 * x
  collectgarbage"restart"
end


if T then   -- tests for weird cases collecting upvalues

  local function foo ()
    local a = {x = 20}
    coroutine.yield(function () return a.x end)  -- will run collector
    assert(a.x == 20)   -- 'a' is 'ok'
    a = {x = 30}   -- create a new object
    assert(T.gccolor(a) == "white")   -- of course it is new...
    coroutine.yield(100)   -- 'a' is still local to this thread
  end

  local t = setmetatable({}, {__mode = "kv"})
  collectgarbage(); collectgarbage('stop')
  -- create coroutine in a weak table, so it will never be marked
  t.co = coroutine.wrap(foo)
  local f = t.co()   -- create function to access local 'a'
  T.gcstate("atomic")   -- ensure all objects are traversed
  assert(T.gcstate() == "atomic")
  assert(t.co() == 100)   -- resume coroutine, creating new table for 'a'
  assert(T.gccolor(t.co) == "white")  -- thread was not traversed
  T.gcstate("pause")   -- collect thread, but should mark 'a' before that
  assert(t.co == nil and f() == 30)   -- ensure correct access to 'a'

  collectgarbage("restart")

  -- test barrier in sweep phase (backing userdata to gray)
  local u = T.newuserdata(0, 1)   -- create a userdata
  collectgarbage()
  collectgarbage"stop"
  local a = {}     -- avoid 'u' as first element in 'allgc'
  T.gcstate"atomic"
  T.gcstate"sweepallgc"
  local x = {}
  assert(T.gccolor(u) == "black")   -- userdata is "old" (black)
  assert(T.gccolor(x) == "white")   -- table is "new" (white)
  debug.setuservalue(u, x)          -- trigger barrier
  assert(T.gccolor(u) == "gray")   -- userdata changed back to gray
  collectgarbage"restart"

  print"+"
end


if T then
  local debug = require "debug"
  collectgarbage("stop")
  local x = T.newuserdata(0)
  local y = T.newuserdata(0)
  debug.setmetatable(y, {__gc = nop})   -- bless the new udata before...
  debug.setmetatable(x, {__gc = nop})   -- ...the old one
  assert(T.gccolor(y) == "white")
  T.checkmemory()
  collectgarbage("restart")
end


if T then
  print("emergency collections")
  collectgarbage()
  collectgarbage()
  T.totalmem(T.totalmem() + 200)
  for i=1,200 do local a = {} end
  T.totalmem(0)
  collectgarbage()
  local t = T.totalmem("table")
  local a = {{}, {}, {}}   -- create 4 new tables
  assert(T.totalmem("table") == t + 4)
  t = T.totalmem("function")
  a = function () end   -- create 1 new closure
  assert(T.totalmem("function") == t + 1)
  t = T.totalmem("thread")
  a = coroutine.create(function () end)   -- create 1 new coroutine
  assert(T.totalmem("thread") == t + 1)
end


-- create an object to be collected when state is closed
do
  local setmetatable,assert,type,print,getmetatable =
        setmetatable,assert,type,print,getmetatable
  local tt = {}
  tt.__gc = function (o)
    assert(getmetatable(o) == tt)
    -- create new objects during GC
    local a = 'xuxu'..(10+3)..'joao', {}
    ___Glob = o  -- ressurrect object!
    setmetatable({}, tt)  -- creates a new one with same metatable
    print(">>> closing state " .. "<<<\n")
  end
  local u = setmetatable({}, tt)
  ___Glob = {u}   -- avoid object being collected before program end
end

-- create several objects to raise errors when collected while closing state
if T then
  local error, assert, find, warn = error, assert, string.find, warn
  local n = 0
  local lastmsg
  local mt = {__gc = function (o)
    n = n + 1
    assert(n == o[1])
    if n == 1 then
      _WARN = false
    elseif n == 2 then
      assert(find(_WARN, "@expected warning"))
      lastmsg = _WARN    -- get message from previous error (first 'o')
    else
      assert(lastmsg == _WARN)  -- subsequent error messages are equal
    end
    warn("@store"); _WARN = false
    error"@expected warning"
  end}
  for i = 10, 1, -1 do
    -- create object and preserve it until the end
    table.insert(___Glob, setmetatable({i}, mt))
  end
end

-- just to make sure
assert(collectgarbage'isrunning')

do    -- check that the collector is reentrant in incremental mode
  setmetatable({}, {__gc = function ()
    collectgarbage()
  end})
  collectgarbage()
end


collectgarbage(oldmode)

print('OK')

-- $Id: testes/gengc.lua $
-- See Copyright Notice in file all.lua

print('testing generational garbage collection')

local debug = require"debug"

assert(collectgarbage("isrunning"))

collectgarbage()

local oldmode = collectgarbage("generational")


-- ensure that table barrier evolves correctly
do
  local U = {}
  -- full collection makes 'U' old
  collectgarbage()
  assert(not T or T.gcage(U) == "old")

  -- U refers to a new table, so it becomes 'touched1'
  U[1] = {x = {234}}
  assert(not T or (T.gcage(U) == "touched1" and T.gcage(U[1]) == "new"))

  -- both U and the table survive one more collection
  collectgarbage("step", 0)
  assert(not T or (T.gcage(U) == "touched2" and T.gcage(U[1]) == "survival"))

  -- both U and the table survive yet another collection
  -- now everything is old
  collectgarbage("step", 0)
  assert(not T or (T.gcage(U) == "old" and T.gcage(U[1]) == "old1"))

  -- data was not corrupted
  assert(U[1].x[1] == 234)
end


do
  -- ensure that 'firstold1' is corrected when object is removed from
  -- the 'allgc' list
  local function foo () end
  local old = {10}
  collectgarbage()    -- make 'old' old
  assert(not T or T.gcage(old) == "old")
  setmetatable(old, {})    -- new table becomes OLD0 (barrier)
  assert(not T or T.gcage(getmetatable(old)) == "old0")
  collectgarbage("step", 0)   -- new table becomes OLD1 and firstold1
  assert(not T or T.gcage(getmetatable(old)) == "old1")
  setmetatable(getmetatable(old), {__gc = foo})  -- get it out of allgc list
  collectgarbage("step", 0)   -- should not seg. fault
end


do   -- bug in 5.4.0
-- When an object aged OLD1 is finalized, it is moved from the list
-- 'finobj' to the *beginning* of the list 'allgc', but that part of the
-- list was not being visited by 'markold'.
  local A = {}
  A[1] = false     -- old anchor for object

  -- obj finalizer
  local function gcf (obj)
    A[1] = obj     -- anchor object
    assert(not T or T.gcage(obj) == "old1")
    obj = nil      -- remove it from the stack
    collectgarbage("step", 0)   -- do a young collection
    print(getmetatable(A[1]).x)   -- metatable was collected
  end

  collectgarbage()   -- make A old
  local obj = {}     -- create a new object
  collectgarbage("step", 0)   -- make it a survival
  assert(not T or T.gcage(obj) == "survival")
  setmetatable(obj, {__gc = gcf, x = "+"})   -- create its metatable
  assert(not T or T.gcage(getmetatable(obj)) == "new")
  obj = nil   -- clear object
  collectgarbage("step", 0)   -- will call obj's finalizer
end


do   -- another bug in 5.4.0
  local old = {10}
  collectgarbage()   -- make 'old' old
  local co = coroutine.create(
    function ()
      local x = nil
      local f = function ()
                  return x[1]
                end
      x = coroutine.yield(f)
      coroutine.yield()
    end
  )
  local _, f = coroutine.resume(co)   -- create closure over 'x' in coroutine
  collectgarbage("step", 0)   -- make upvalue a survival
  old[1] = {"hello"}    -- 'old' go to grayagain as 'touched1'
  coroutine.resume(co, {123})     -- its value will be new
  co = nil
  collectgarbage("step", 0)   -- hit the barrier
  assert(f() == 123 and old[1][1] == "hello")
  collectgarbage("step", 0)   -- run the collector once more
  -- make sure old[1] was not collected
  assert(f() == 123 and old[1][1] == "hello")
end


do   -- bug introduced in commit 9cf3299fa
  local t = setmetatable({}, {__mode = "kv"})   -- all-weak table
  collectgarbage()   -- full collection
  assert(not T or T.gcage(t) == "old")
  t[1] = {10}
  assert(not T or (T.gcage(t) == "touched1" and T.gccolor(t) == "gray"))
  collectgarbage("step", 0)   -- minor collection
  assert(not T or (T.gcage(t) == "touched2" and T.gccolor(t) == "black"))
  collectgarbage("step", 0)   -- minor collection
  assert(not T or T.gcage(t) == "old")   -- t should be black, but it was gray
  t[1] = {10}      -- no barrier here, so t was still old
  collectgarbage("step", 0)   -- minor collection
  -- t, being old, is ignored by the collection, so it is not cleared
  assert(t[1] == nil)   -- fails with the bug
end


if T == nil then
  (Message or print)('\n >>> testC not active: \z
                             skipping some generational tests <<<\n')
  print 'OK'
  return
end


-- ensure that userdata barrier evolves correctly
do
  local U = T.newuserdata(0, 1)
  -- full collection makes 'U' old
  collectgarbage()
  assert(T.gcage(U) == "old")

  -- U refers to a new table, so it becomes 'touched1'
  debug.setuservalue(U, {x = {234}})
  assert(T.gcage(U) == "touched1" and
         T.gcage(debug.getuservalue(U)) == "new")

  -- both U and the table survive one more collection
  collectgarbage("step", 0)
  assert(T.gcage(U) == "touched2" and
         T.gcage(debug.getuservalue(U)) == "survival")

  -- both U and the table survive yet another collection
  -- now everything is old
  collectgarbage("step", 0)
  assert(T.gcage(U) == "old" and
         T.gcage(debug.getuservalue(U)) == "old1")

  -- data was not corrupted
  assert(debug.getuservalue(U).x[1] == 234)
end

-- just to make sure
assert(collectgarbage'isrunning')



-- just to make sure
assert(collectgarbage'isrunning')

collectgarbage(oldmode)

print('OK')

-- $Id: testes/goto.lua $
-- See Copyright Notice in file all.lua

collectgarbage()

local function errmsg (code, m)
  local st, msg = load(code)
  assert(not st and string.find(msg, m))
end

-- cannot see label inside block
errmsg([[ goto l1; do ::l1:: end ]], "label 'l1'")
errmsg([[ do ::l1:: end goto l1; ]], "label 'l1'")

-- repeated label
errmsg([[ ::l1:: ::l1:: ]], "label 'l1'")
errmsg([[ ::l1:: do ::l1:: end]], "label 'l1'")


-- undefined label
errmsg([[ goto l1; local aa ::l1:: ::l2:: print(3) ]], "local 'aa'")

-- jumping over variable definition
errmsg([[
do local bb, cc; goto l1; end
local aa
::l1:: print(3)
]], "local 'aa'")

-- jumping into a block
errmsg([[ do ::l1:: end goto l1 ]], "label 'l1'")
errmsg([[ goto l1 do ::l1:: end ]], "label 'l1'")

-- cannot continue a repeat-until with variables
errmsg([[
  repeat
    if x then goto cont end
    local xuxu = 10
    ::cont::
  until xuxu < x
]], "local 'xuxu'")

-- simple gotos
local x
do
  local y = 12
  goto l1
  ::l2:: x = x + 1; goto l3
  ::l1:: x = y; goto l2
end
::l3:: ::l3_1:: assert(x == 13)


-- long labels
do
  local prog = [[
  do
    local a = 1
    goto l%sa; a = a + 1
   ::l%sa:: a = a + 10
    goto l%sb; a = a + 2
   ::l%sb:: a = a + 20
    return a
  end
  ]]
  local label = string.rep("0123456789", 40)
  prog = string.format(prog, label, label, label, label)
  assert(assert(load(prog))() == 31)
end


-- ok to jump over local dec. to end of block
do
  goto l1
  local a = 23
  x = a
  ::l1::;
end

while true do
  goto l4
  goto l1  -- ok to jump over local dec. to end of block
  goto l1  -- multiple uses of same label
  local x = 45
  ::l1:: ;;;
end
::l4:: assert(x == 13)

if print then
  goto l1   -- ok to jump over local dec. to end of block
  error("should not be here")
  goto l2   -- ok to jump over local dec. to end of block
  local x
  ::l1:: ; ::l2:: ;;
else end

-- to repeat a label in a different function is OK
local function foo ()
  local a = {}
  goto l3
  ::l1:: a[#a + 1] = 1; goto l2;
  ::l2:: a[#a + 1] = 2; goto l5;
  ::l3::
  ::l3a:: a[#a + 1] = 3; goto l1;
  ::l4:: a[#a + 1] = 4; goto l6;
  ::l5:: a[#a + 1] = 5; goto l4;
  ::l6:: assert(a[1] == 3 and a[2] == 1 and a[3] == 2 and
              a[4] == 5 and a[5] == 4)
  if not a[6] then a[6] = true; goto l3a end   -- do it twice
end

::l6:: foo()


do   -- bug in 5.2 -> 5.3.2
  local x
  ::L1::
  local y             -- cannot join this SETNIL with previous one
  assert(y == nil)
  y = true
  if x == nil then
    x = 1
    goto L1
  else
    x = x + 1
  end
  assert(x == 2 and y == true)
end

-- bug in 5.3
do
  local first = true
  local a = false
  if true then
    goto LBL
    ::loop::
    a = true
    ::LBL::
    if first then
      first = false
      goto loop
    end
  end
  assert(a)
end

do   -- compiling infinite loops
  goto escape   -- do not run the infinite loops
  ::a:: goto a
  ::b:: goto c
  ::c:: goto b
end
::escape::
--------------------------------------------------------------------------------
-- testing closing of upvalues

local debug = require 'debug'

local function foo ()
  local t = {}
  do
  local i = 1
  local a, b, c, d
  t[1] = function () return a, b, c, d end
  ::l1::
  local b
  do
    local c
    t[#t + 1] = function () return a, b, c, d end    -- t[2], t[4], t[6]
    if i > 2 then goto l2 end
    do
      local d
      t[#t + 1] = function () return a, b, c, d end   -- t[3], t[5]
      i = i + 1
      local a
      goto l1
    end
  end
  end
  ::l2:: return t
end

local a = foo()
assert(#a == 6)

-- all functions share same 'a'
for i = 2, 6 do
  assert(debug.upvalueid(a[1], 1) == debug.upvalueid(a[i], 1))
end

-- 'b' and 'c' are shared among some of them
for i = 2, 6 do
  -- only a[1] uses external 'b'/'b'
  assert(debug.upvalueid(a[1], 2) ~= debug.upvalueid(a[i], 2))
  assert(debug.upvalueid(a[1], 3) ~= debug.upvalueid(a[i], 3))
end

for i = 3, 5, 2 do
  -- inner functions share 'b'/'c' with previous ones
  assert(debug.upvalueid(a[i], 2) == debug.upvalueid(a[i - 1], 2))
  assert(debug.upvalueid(a[i], 3) == debug.upvalueid(a[i - 1], 3))
  -- but not with next ones
  assert(debug.upvalueid(a[i], 2) ~= debug.upvalueid(a[i + 1], 2))
  assert(debug.upvalueid(a[i], 3) ~= debug.upvalueid(a[i + 1], 3))
end

-- only external 'd' is shared
for i = 2, 6, 2 do
  assert(debug.upvalueid(a[1], 4) == debug.upvalueid(a[i], 4))
end

-- internal 'd's are all different
for i = 3, 5, 2 do
  for j = 1, 6 do
    assert((debug.upvalueid(a[i], 4) == debug.upvalueid(a[j], 4))
      == (i == j))
  end
end

--------------------------------------------------------------------------------
-- testing if x goto optimizations

local function testG (a)
  if a == 1 then
    goto l1
    error("should never be here!")
  elseif a == 2 then goto l2
  elseif a == 3 then goto l3
  elseif a == 4 then
    goto l1  -- go to inside the block
    error("should never be here!")
    ::l1:: a = a + 1   -- must go to 'if' end
  else
    goto l4
    ::l4a:: a = a * 2; goto l4b
    error("should never be here!")
    ::l4:: goto l4a
    error("should never be here!")
    ::l4b::
  end
  do return a end
  ::l2:: do return "2" end
  ::l3:: do return "3" end
  ::l1:: return "1"
end

assert(testG(1) == "1")
assert(testG(2) == "2")
assert(testG(3) == "3")
assert(testG(4) == 5)
assert(testG(5) == 10)

do
  -- if x back goto out of scope of upvalue
  local X
  goto L1

  ::L2:: goto L3

  ::L1:: do
    local a <close> = setmetatable({}, {__close = function () X = true end})
    assert(X == nil)
    if a then goto L2 end   -- jumping back out of scope of 'a'
  end

  ::L3:: assert(X == true)   -- checks that 'a' was correctly closed
end
--------------------------------------------------------------------------------


print'OK'
