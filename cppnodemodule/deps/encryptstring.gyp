{ 
	'targets': [
    {
        'target_name': 'encryptstring',
        'type': 'static_library',
		'includes':[
			'./config.gypi',
		],
        'include_dirs': [
			'boost_1_57_0',
            'encryptstring',
        ],
		'sources': [
            "encryptstring/aes.h",
            "encryptstring/aes.c",
            "encryptstring/aes-core.h",
            "encryptstring/aes-core.c",
            "encryptstring/md5.cc",
            "encryptstring/md5.h",
            "encryptstring/encryptstring.cc",
            "encryptstring/encryptstring.h"
		],
    },
	]
}
