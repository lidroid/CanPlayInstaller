-- the debug mode
if is_mode("debug") then
    
    -- enable the debug symbols
    set_symbols("debug")

    -- disable optimization
    set_optimize("none")
end

-- the release mode
if is_mode("release") then

    -- set the symbols visibility: hidden
    set_symbols("hidden")

    -- enable fastest optimization
    set_optimize("fastest")

    -- strip all symbols
    set_strip("all")
end

-- the parameters for compile and build
if is_plat("macosx") then
    add_ldflags("-framework Foundation", "-framework CoreFoundation")
    --add_ldflags("-fobjc-arc", "-fobjc-link-runtime");
    add_mxxflags("-std=c++1z")
elseif is_plat("windows") then
    add_defines("UNICODE", "_UNICODE", "WINDOWS", "_WINDOWS", "WIN32", "_WIN32")
    if is_mode("release") then
        add_defines("NDEBUG")
        add_cxxflags("/GL");
    end
    add_cxxflags("/Zc:wchar_t", "/Zc:inline", "/Gd", "/EHsc", "/Od", "/Oi", "/Oy-")
    add_cxxflags("/Gy", "/Gm-", "/Zc:forScope", "/analyze-", "/fp:fast", "/WX-")
    add_ldflags("/OPT:REF", "/OPT:ICF", "/LTCG:incremental", "/MACHINE:X86", "/SAFESEH")
    add_ldflags("/DYNAMICBASE", "/INCREMENTAL:NO", "/SUBSYSTEM:WINDOWS,5.01")
    add_ldflags("kernel32.lib", "user32.lib", "gdi32.lib", "winspool.lib", "comdlg32.lib")
    add_ldflags("advapi32.lib", "shell32.lib", "ole32.lib", "oleaut32.lib", "uuid.lib")
end

-- add target
target("CanPlayInstaller")
set_kind("binary")
add_defines("ASIO_STANDALONE", "ASIO_HAS_BOOST_DATE_TIME", "ELPP_THREAD_SAFE")
add_cxxflags("-std=c++11")
add_includedirs("src", "sciter/include", "asio", "easyloggingcpp", "boost", "cryptopp/include")
add_files("src/*.cpp")
add_files("easyloggingcpp/*.cc")
add_files("sciter/include/behaviors/behavior_tabs.cpp")
add_files("sciter/include/behaviors/behavior_video_generator.cpp")
add_files("sciter/include/behaviors/behavior_drawing.cpp")
if is_plat("macosx") then
    add_ldflags("-framework Cocoa", "-framework WebKit")
    add_ldflags("sciter/osx/sciter-osx-64.dylib")
    add_ldflags("boost/osx/libboost_system.a", "boost/osx/libboost_filesystem.a")
    add_ldflags("boost/osx/libboost_date_time.a")
    add_ldflags("cryptopp/osx/libcryptopp.a")
    add_mxxflags("-std=c++11")
    add_files("src/webview/*.mm")
    add_files("sciter/include/sciter-osx-main.mm")
elseif is_plat("windows") then
    add_includedirs("src/win", "vbox/mscom/include", "vmp/include")
    add_ldflags("ws2_32.lib")
    add_ldflags("boost/win32/libboost_system-vc141-mt-s-1_64.lib")
    add_ldflags("boost/win32/libboost_filesystem-vc141-mt-s-1_64.lib")
    add_ldflags("boost/win32/libboost_date_time-vc141-mt-s-1_64.lib")
    add_ldflags("cryptopp/win32/cryptlib.lib")
    add_files("src/webview/*.cpp")
    add_files("src/win/app.rc")
    add_files("sciter/include/sciter-win-main.cpp")
    add_files("sciter/include/behaviors/behavior_drawing-opengl.cpp")
    add_files("sciter/include/behaviors/behavior_drawing-gdi.cpp")
    after_build(function()
        os.run("src/win/mt.exe -manifest src/win/PerMonitorHighDPIAware.manifest -outputresource:build/CanPlayInstaller.exe;1")
    end);
end
