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
            "src/win.cc"
          ]
        }, { # "OS!='win"
          "include_dirs": [ "../deps/ncurses" ],
          "link_settings": {
            "libraries": [ "-lncurses++", "-lncurses" ],
            "library_dirs": [ "../deps/ncurses/lib/" ]
          },
          "sources": [
            "src/unix.cc"
          ]
        }]
      ]
    }
  ]
}
