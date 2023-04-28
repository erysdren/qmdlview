# qmdlview

![an image of the final boss from quake](.github/qmdlview.png)

a software-rendered quake model viewer written in C. currently only supports standard version 6 quake mdl files.

it can run on modern platforms with SDL2 or on DOS with VGA mode 13.

## todo

- [ ] add support for onseam texture coords
- [ ] add support for multiple frames and skins
- [ ] add support for qtest (version 3) models
- [ ] add support for quake 2 (version 8) models
- [x] add a dos platform target
- [ ] add some basic editing features
- [ ] clean up mdl.c

## thirdparty

* [small3dlib](https://gitlab.com/drummyfish/small3dlib)
* [font8x8](https://github.com/dhepper/font8x8)
* [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/)

## license

ANTI-CAPITALIST SOFTWARE LICENSE (v 1.4)

Copyright © 2023 erysdren (it/they/she)

This is anti-capitalist software, released for free use by individuals
and organizations that do not operate by capitalist principles.

Permission is hereby granted, free of charge, to any person or
organization (the "User") obtaining a copy of this software and
associated documentation files (the "Software"), to use, copy, modify,
merge, distribute, and/or sell copies of the Software, subject to the
following conditions:

  1. The above copyright notice and this permission notice shall be
  included in all copies or modified versions of the Software.

  2. The User is one of the following:
    a. An individual person, laboring for themselves
    b. A non-profit organization
    c. An educational institution
    d. An organization that seeks shared profit for all of its members,
    and allows non-members to set the cost of their labor

  3. If the User is an organization with owners, then all owners are
  workers and all workers are owners with equal equity and/or equal vote.

  4. If the User is an organization, then the User is not law enforcement
  or military, or working for or under either.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT EXPRESS OR IMPLIED WARRANTY OF
ANY KIND, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
