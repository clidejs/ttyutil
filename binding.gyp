{
  "targets": [
    {
      "target_name": "ttyu",
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
        "deps/ee.c/",
        "include/"
      ],
      "sources": [
        "deps/ee.c/ee.c",
        "src/core/ttyu_event.cc",
        "src/core/ttyu.cc",
        "src/core/utils.cc"
      ],

      # pre install script
      "actions": [
        {
          "action_name": "preinstall",
          "inputs": [ "tools/build.js" ],
          "outputs": [ "" ],
          "action": [ "node", "tools/build.js" ]
        }
      ],

      "conditions": [
        ["OS=='win'", {
          "defines": [
            "PLATFORM_WINDOWS"
          ],
          "sources": [
            "src/win/main.cc" # TODO
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
            "src/unix/off.cc",
            "src/unix/on.cc",
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
