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
                    "sources": [
                        "src/impl/unix.cc"
                    ]
                }]
            ]
        }
    ]
}
