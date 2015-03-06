{
    "targets": [
        {
            "target_name": "ttyu",
            "include_dirs" : [
                "<!(node -e \"require('nan')\")",
                "deps/ee/",
                "include/"
            ],
            "sources": [
                "deps/ee/ee.c",
                "src/ttyu_event.cc",
                "src/ttyu_worker.cc",
                "src/ttyu_js.cc",
                "src/ttyu.cc"
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
                        "-lcurses" # add ncurses compiler flag
                    ],
                    "sources": [
                        "src/unix.cc"
                    ]
                }]
            ]
        }
    ]
}
