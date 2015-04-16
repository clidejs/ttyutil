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
                "src/ttyu_js.cc",
                "src/ttyu.cc",
                "src/utils.cc"
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
                    "libraries": [
                        "-lcurses" # add curses compiler flag
                    ],
                    "sources": [
                        "src/unix.cc"
                    ]
                }]
            ]
        }
    ]
}
