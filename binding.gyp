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
        "src/ttyu_event.cc",
        "src/ttyu.cc",
        "src/utils.cc"
      ],
      "actions": [
        {
          "action_name": "ncurses_build",
          "inputs": [ "build.js" ],
          "outputs": [ "" ],
          "action": [ "node", "build.js" ]
        }
      ],
      "conditions": [
        ["OS=='win'", {
          "defines": [
            "PLATFORM_WINDOWS"
          ],
          "sources": [
            "src/win.cc"
          ]
        }, { # "OS!='win"
          "include_dirs": [ "deps/ncurses" ],
          "link_settings": {
            "libraries": [ "-lncurses++", "-lncurses" ],
            "library_dirs": [ "deps/ncurses/lib" ]
          },
          "sources": [
            "src/unix.cc"
          ]
        }]
      ]
    }
  ]
}
