/*----------------------------------------------------------------------------
 ChuGL: Unified Audiovisual Programming in ChucK

 Copyright (c) 2023 Andrew Zhu Aday and Ge Wang. All rights reserved.
   http://chuck.stanford.edu/chugl/
   http://chuck.cs.princeton.edu/chugl/

 MIT License
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
-----------------------------------------------------------------------------*/
// standalone main for testing the renderer only
// does NOT link with chuck or chugin.h in any way
// TODO: rename this to "test_all"

#include "all.cpp"

// #include "tests/gltf.cpp"
// #include "tests/obj.cpp"

// void Test_Obj(TestCallbacks* callbacks);
void Test_Gltf(TestCallbacks* callbacks);
void Test_ImGUI(TestCallbacks* callbacks);
void Test_Box2d(TestCallbacks* callbacks);
void Test_Lines2D(TestCallbacks* callbacks);

int main(int, char**)
{
    log_trace("main");
    App app        = {};
    app.standalone = true;

    // load test entry points
    // Test_Obj(&app.callbacks);
    // Test_Gltf(&app.callbacks);
    // Test_ImGUI(&app.callbacks);
    // Test_Box2d(&app.callbacks);
    Test_Lines2D(&app.callbacks);

    App::init(&app, NULL, NULL);
    App::start(&app);
    App::end(&app);

    return EXIT_SUCCESS;
}
