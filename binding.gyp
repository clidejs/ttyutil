{
    "targets": [
        {
            "target_name": "tty",
            "include_dirs" : [
                "<!(node -e \"require('nan')\")"
            ],
            "sources": [
                "src/tty.cc"
            ],
            "conditions": [
                ["OS=='win'", {
                    "defines": [
                        "PLATFORM_WINDOWS"
                    ],
                    "sources": [
                        "src/impl/win.cc"
                    ]
                }, { # "OS!='win"
                    "libraries": [
                        "-lcurses" # add ncurses compiler flag
                    ],
                    "sources": [
                        "src/impl/unix.cc"
                    ]
                }],
                ["OS=='mac'", {
                    "xcode_settings": {
                        "GCC_ENABLE_CPP_RTTI": "YES"
                    }
                }]
            ]
        }
    ]
}
