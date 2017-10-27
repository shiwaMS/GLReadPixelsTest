# GLReadPixelsTest

Build the Android project, find *.so file under 
<code><$ProjectDir>\app\build\intermediates\cmake\release\obj</code>

Copy both armeabi-v7a and x86 folder to Unity project's plugin folders:
<code>Assets\Plugins\Android\libs</code>

The the plugin folder of Unity project will be:
<code>Assets\Plugins\Android\libs\armeabi-v7a\libScreenPointPixel-lib.so</code>.

and
<code>Assets\Plugins\Android\libs\x86\libScreenPointPixel-lib.so</code>.

