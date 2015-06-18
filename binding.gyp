{
  "targets": [
    {
      "target_name": "ttyu",
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
        "include/"
      ],
      "sources": [
        "src/core/ttyu_event.cc",
        "src/core/ttyu.cc"
      ],

      # pre install script
      "actions": [
        {
          "action_name": "preinstall",
          "inputs": [ "tools/build.js" ],
          "outputs": [ "include/generated.h" ],
          "action": [ "node", "tools/build.js" ]
        }
      ],

      "conditions": [
        ["OS=='win'", {
          "defines": [
            "PLATFORM_WINDOWS"
          ],
          "sources": [
            "src/win/beep.cc",
            "src/win/clear.cc",
            "src/win/emit.cc",
            "src/win/goto.cc",
            "src/win/mode.cc",
            "src/win/colors.cc",
            "src/win/resize.cc",
            "src/win/start.cc",
            "src/win/stop.cc",
            "src/win/write.cc",
            "src/win/main.cc",
            "src/win/utils.cc",
            "src/win/worker.cc"
          ]
        }, { # "OS!='win"
          "include_dirs": [ "../deps/ncurses" ],
          "link_settings": {
            "libraries": [ "-lncurses++", "-lncurses" ],
            "library_dirs": [ "../deps/ncurses/lib/" ]
          },
          "sources": [
            "src/unix/beep.cc",
            "src/unix/clear.cc",
            "src/unix/emit.cc",
            "src/unix/goto.cc",
            "src/unix/mode.cc",
            "src/unix/colors.cc",
            "src/unix/resize.cc",
            "src/unix/start.cc",
            "src/unix/stop.cc",
            "src/unix/write.cc",
            "src/unix/main.cc",
            "src/unix/utils.cc",
            "src/unix/curses.cc",
            "src/unix/worker.cc"
          ]
        }]
      ]
    }
  ]
}
