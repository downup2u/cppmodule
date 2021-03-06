# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: iteasycomm.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)




DESCRIPTOR = _descriptor.FileDescriptor(
  name='iteasycomm.proto',
  package='iteasycomm',
  serialized_pb='\n\x10iteasycomm.proto\x12\niteasycomm\"\x15\n\x07PkgPing\x12\n\n\x02id\x18\x01 \x02(\x05\"@\n\rPkgConnectReq\x12\x10\n\x08username\x18\x01 \x02(\t\x12\x10\n\x08serverid\x18\x02 \x02(\t\x12\x0b\n\x03sn1\x18\x03 \x02(\t\"q\n\x0fPkgConnectReply\x12\x11\n\tissuccess\x18\x01 \x02(\x08\x12\x0b\n\x03\x65rr\x18\x02 \x02(\t\x12\x12\n\nsessionkey\x18\x03 \x02(\t\x12\x10\n\x08serverid\x18\x04 \x02(\t\x12\x0b\n\x03\x66n1\x18\x05 \x02(\t\x12\x0b\n\x03sn2\x18\x06 \x02(\t\"p\n\x0bPkgLoginReq\x12\x10\n\x08username\x18\x01 \x02(\t\x12\x10\n\x08password\x18\x02 \x02(\t\x12\x14\n\x0cpolicylastid\x18\x03 \x01(\t\x12\x1a\n\x12policyinlinelastid\x18\x04 \x01(\t\x12\x0b\n\x03\x66n2\x18\x05 \x02(\t\"\x9b\x01\n\rPkgLoginReply\x12\x11\n\tissuccess\x18\x01 \x02(\x08\x12\x0b\n\x03\x65rr\x18\x02 \x02(\t\x12\x10\n\x08sfilekey\x18\x03 \x02(\t\x12\x12\n\nservertime\x18\x04 \x02(\t\x12\x17\n\x0freqpinginterval\x18\x05 \x02(\x05\x12\x13\n\x0b\x63ompanyname\x18\x06 \x01(\t\x12\x16\n\x0e\x64\x65partmentname\x18\x07 \x01(\t\"\x88\x05\n\nStruLogReq\x12\n\n\x02id\x18\x01 \x02(\x05\x12\x0f\n\x07logtime\x18\x02 \x02(\t\x12?\n\x07logtype\x18\x03 \x02(\x0e\x32\".iteasycomm.StruLogReq.LogTypeEnum:\nLOGTYPE_FS\x12@\n\x08loglevel\x18\x04 \x02(\x0e\x32\x1f.iteasycomm.StruLogReq.LogLevel:\rLOGLEVEL_INFO\x12\x12\n\nlogcontent\x18\x05 \x02(\t\x12\x13\n\x08issendui\x18\x06 \x01(\x05:\x01\x30\x12<\n\x06logcmd\x18\x07 \x01(\x0e\x32\x1d.iteasycomm.StruLogReq.LogCmd:\rLOGCMD_NORMAL\"I\n\x0bLogTypeEnum\x12\x0e\n\nLOGTYPE_FS\x10\x01\x12\x15\n\x11LOGTYPE_CLIPBOARD\x10\x02\x12\x13\n\x0fLOGTYPE_PRINTER\x10\x03\"c\n\x08LogLevel\x12\x11\n\rLOGLEVEL_INFO\x10\x00\x12\x14\n\x10LOGLEVEL_WARNING\x10\x01\x12\x12\n\x0eLOGLEVEL_ERROR\x10\x02\x12\x1a\n\x16LOGLEVEL_AGAINSTPOLICY\x10\n\"\xc2\x01\n\x06LogCmd\x12\x11\n\rLOGCMD_NORMAL\x10\x00\x12\x12\n\x0eLOGCMD_ENABLED\x10\x01\x12\x13\n\x0fLOGCMD_DISABLED\x10\x02\x12\x1f\n\x1bLOGCMD_ENCRYPTFILENOTCOMANY\x10\x03\x12$\n LOGCMD_ENCRYPTFILENOTDEPARTEMENT\x10\x04\x12\x1b\n\x17LOGCMD_DISABLEDINPOLICY\x10\x05\x12\x18\n\x14LOGCMD_DISABLERENAME\x10\x06\"7\n\tPkgLogReq\x12*\n\nlogreqlist\x18\x01 \x03(\x0b\x32\x16.iteasycomm.StruLogReq\"-\n\x0cStruLogReply\x12\n\n\x02id\x18\x01 \x02(\x05\x12\x11\n\tissuccess\x18\x02 \x02(\x08\"=\n\x0bPkgLogReply\x12.\n\x0clogreplylist\x18\x01 \x03(\x0b\x32\x18.iteasycomm.StruLogReply\"\x8b\x02\n\rPkgPolicyPush\x12\x14\n\x0cpolicylastid\x18\x01 \x02(\t\x12\x19\n\x11policyprocessname\x18\x02 \x02(\t\x12 \n\x18policydisableprocessname\x18\x03 \x02(\t\x12)\n!policyallowencryptreadprocessname\x18\x04 \x02(\t\x12\x19\n\x11policyextfilename\x18\x05 \x02(\t\x12\x19\n\x11policyofflinetime\x18\x06 \x02(\x05\x12\x1b\n\x10ingoredepartment\x18\x07 \x01(\x05:\x01\x31\x12)\n!policyallowdecryptreadprocessname\x18\x08 \x01(\t\"8\n\rPkgPolicyEcho\x12\x14\n\x0cpolicylastid\x18\x01 \x02(\t\x12\x11\n\tissuccess\x18\x02 \x02(\x08\"K\n\x13PkgPolicyinlinePush\x12\x1a\n\x12policyinlinelastid\x18\x01 \x02(\t\x12\x18\n\x10policyinlinefile\x18\x02 \x02(\t\">\n\x13PkgPolicyinlineEcho\x12\x14\n\x0cpolicylastid\x18\x01 \x02(\t\x12\x11\n\tissuccess\x18\x02 \x02(\x08\"\xc1\x01\n\x13PkgFileApprovedPush\x12\x11\n\tsrvfileid\x18\x01 \x02(\t\x12\x14\n\x0c\x63lientfileid\x18\x02 \x02(\x05\x12\x0e\n\x06status\x18\x03 \x02(\x05\x12\x12\n\nstatusdesc\x18\x04 \x02(\t\x12\x16\n\x0esrvapprovedmsg\x18\x05 \x02(\t\x12\x17\n\x0fsrvapprovedtime\x18\x06 \x02(\t\x12\x1b\n\x13srvapprovedusername\x18\x07 \x02(\t\x12\x0f\n\x07\x66ilemd5\x18\x08 \x02(\t\";\n\x13PkgFileApprovedEcho\x12\x11\n\tsrvfileid\x18\x01 \x02(\t\x12\x11\n\tissuccess\x18\x02 \x02(\x08')



_STRULOGREQ_LOGTYPEENUM = _descriptor.EnumDescriptor(
  name='LogTypeEnum',
  full_name='iteasycomm.StruLogReq.LogTypeEnum',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='LOGTYPE_FS', index=0, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGTYPE_CLIPBOARD', index=1, number=2,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGTYPE_PRINTER', index=2, number=3,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=786,
  serialized_end=859,
)

_STRULOGREQ_LOGLEVEL = _descriptor.EnumDescriptor(
  name='LogLevel',
  full_name='iteasycomm.StruLogReq.LogLevel',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='LOGLEVEL_INFO', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGLEVEL_WARNING', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGLEVEL_ERROR', index=2, number=2,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGLEVEL_AGAINSTPOLICY', index=3, number=10,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=861,
  serialized_end=960,
)

_STRULOGREQ_LOGCMD = _descriptor.EnumDescriptor(
  name='LogCmd',
  full_name='iteasycomm.StruLogReq.LogCmd',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='LOGCMD_NORMAL', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGCMD_ENABLED', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGCMD_DISABLED', index=2, number=2,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGCMD_ENCRYPTFILENOTCOMANY', index=3, number=3,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGCMD_ENCRYPTFILENOTDEPARTEMENT', index=4, number=4,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGCMD_DISABLEDINPOLICY', index=5, number=5,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGCMD_DISABLERENAME', index=6, number=6,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=963,
  serialized_end=1157,
)


_PKGPING = _descriptor.Descriptor(
  name='PkgPing',
  full_name='iteasycomm.PkgPing',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='iteasycomm.PkgPing.id', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=32,
  serialized_end=53,
)


_PKGCONNECTREQ = _descriptor.Descriptor(
  name='PkgConnectReq',
  full_name='iteasycomm.PkgConnectReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='username', full_name='iteasycomm.PkgConnectReq.username', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='serverid', full_name='iteasycomm.PkgConnectReq.serverid', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='sn1', full_name='iteasycomm.PkgConnectReq.sn1', index=2,
      number=3, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=55,
  serialized_end=119,
)


_PKGCONNECTREPLY = _descriptor.Descriptor(
  name='PkgConnectReply',
  full_name='iteasycomm.PkgConnectReply',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='issuccess', full_name='iteasycomm.PkgConnectReply.issuccess', index=0,
      number=1, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='err', full_name='iteasycomm.PkgConnectReply.err', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='sessionkey', full_name='iteasycomm.PkgConnectReply.sessionkey', index=2,
      number=3, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='serverid', full_name='iteasycomm.PkgConnectReply.serverid', index=3,
      number=4, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='fn1', full_name='iteasycomm.PkgConnectReply.fn1', index=4,
      number=5, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='sn2', full_name='iteasycomm.PkgConnectReply.sn2', index=5,
      number=6, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=121,
  serialized_end=234,
)


_PKGLOGINREQ = _descriptor.Descriptor(
  name='PkgLoginReq',
  full_name='iteasycomm.PkgLoginReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='username', full_name='iteasycomm.PkgLoginReq.username', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='password', full_name='iteasycomm.PkgLoginReq.password', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policylastid', full_name='iteasycomm.PkgLoginReq.policylastid', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policyinlinelastid', full_name='iteasycomm.PkgLoginReq.policyinlinelastid', index=3,
      number=4, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='fn2', full_name='iteasycomm.PkgLoginReq.fn2', index=4,
      number=5, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=236,
  serialized_end=348,
)


_PKGLOGINREPLY = _descriptor.Descriptor(
  name='PkgLoginReply',
  full_name='iteasycomm.PkgLoginReply',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='issuccess', full_name='iteasycomm.PkgLoginReply.issuccess', index=0,
      number=1, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='err', full_name='iteasycomm.PkgLoginReply.err', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='sfilekey', full_name='iteasycomm.PkgLoginReply.sfilekey', index=2,
      number=3, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='servertime', full_name='iteasycomm.PkgLoginReply.servertime', index=3,
      number=4, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='reqpinginterval', full_name='iteasycomm.PkgLoginReply.reqpinginterval', index=4,
      number=5, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='companyname', full_name='iteasycomm.PkgLoginReply.companyname', index=5,
      number=6, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='departmentname', full_name='iteasycomm.PkgLoginReply.departmentname', index=6,
      number=7, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=351,
  serialized_end=506,
)


_STRULOGREQ = _descriptor.Descriptor(
  name='StruLogReq',
  full_name='iteasycomm.StruLogReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='iteasycomm.StruLogReq.id', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='logtime', full_name='iteasycomm.StruLogReq.logtime', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='logtype', full_name='iteasycomm.StruLogReq.logtype', index=2,
      number=3, type=14, cpp_type=8, label=2,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='loglevel', full_name='iteasycomm.StruLogReq.loglevel', index=3,
      number=4, type=14, cpp_type=8, label=2,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='logcontent', full_name='iteasycomm.StruLogReq.logcontent', index=4,
      number=5, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='issendui', full_name='iteasycomm.StruLogReq.issendui', index=5,
      number=6, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='logcmd', full_name='iteasycomm.StruLogReq.logcmd', index=6,
      number=7, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _STRULOGREQ_LOGTYPEENUM,
    _STRULOGREQ_LOGLEVEL,
    _STRULOGREQ_LOGCMD,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=509,
  serialized_end=1157,
)


_PKGLOGREQ = _descriptor.Descriptor(
  name='PkgLogReq',
  full_name='iteasycomm.PkgLogReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='logreqlist', full_name='iteasycomm.PkgLogReq.logreqlist', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1159,
  serialized_end=1214,
)


_STRULOGREPLY = _descriptor.Descriptor(
  name='StruLogReply',
  full_name='iteasycomm.StruLogReply',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='iteasycomm.StruLogReply.id', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='issuccess', full_name='iteasycomm.StruLogReply.issuccess', index=1,
      number=2, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1216,
  serialized_end=1261,
)


_PKGLOGREPLY = _descriptor.Descriptor(
  name='PkgLogReply',
  full_name='iteasycomm.PkgLogReply',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='logreplylist', full_name='iteasycomm.PkgLogReply.logreplylist', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1263,
  serialized_end=1324,
)


_PKGPOLICYPUSH = _descriptor.Descriptor(
  name='PkgPolicyPush',
  full_name='iteasycomm.PkgPolicyPush',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='policylastid', full_name='iteasycomm.PkgPolicyPush.policylastid', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policyprocessname', full_name='iteasycomm.PkgPolicyPush.policyprocessname', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policydisableprocessname', full_name='iteasycomm.PkgPolicyPush.policydisableprocessname', index=2,
      number=3, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policyallowencryptreadprocessname', full_name='iteasycomm.PkgPolicyPush.policyallowencryptreadprocessname', index=3,
      number=4, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policyextfilename', full_name='iteasycomm.PkgPolicyPush.policyextfilename', index=4,
      number=5, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policyofflinetime', full_name='iteasycomm.PkgPolicyPush.policyofflinetime', index=5,
      number=6, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ingoredepartment', full_name='iteasycomm.PkgPolicyPush.ingoredepartment', index=6,
      number=7, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policyallowdecryptreadprocessname', full_name='iteasycomm.PkgPolicyPush.policyallowdecryptreadprocessname', index=7,
      number=8, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1327,
  serialized_end=1594,
)


_PKGPOLICYECHO = _descriptor.Descriptor(
  name='PkgPolicyEcho',
  full_name='iteasycomm.PkgPolicyEcho',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='policylastid', full_name='iteasycomm.PkgPolicyEcho.policylastid', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='issuccess', full_name='iteasycomm.PkgPolicyEcho.issuccess', index=1,
      number=2, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1596,
  serialized_end=1652,
)


_PKGPOLICYINLINEPUSH = _descriptor.Descriptor(
  name='PkgPolicyinlinePush',
  full_name='iteasycomm.PkgPolicyinlinePush',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='policyinlinelastid', full_name='iteasycomm.PkgPolicyinlinePush.policyinlinelastid', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='policyinlinefile', full_name='iteasycomm.PkgPolicyinlinePush.policyinlinefile', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1654,
  serialized_end=1729,
)


_PKGPOLICYINLINEECHO = _descriptor.Descriptor(
  name='PkgPolicyinlineEcho',
  full_name='iteasycomm.PkgPolicyinlineEcho',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='policylastid', full_name='iteasycomm.PkgPolicyinlineEcho.policylastid', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='issuccess', full_name='iteasycomm.PkgPolicyinlineEcho.issuccess', index=1,
      number=2, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1731,
  serialized_end=1793,
)


_PKGFILEAPPROVEDPUSH = _descriptor.Descriptor(
  name='PkgFileApprovedPush',
  full_name='iteasycomm.PkgFileApprovedPush',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='srvfileid', full_name='iteasycomm.PkgFileApprovedPush.srvfileid', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='clientfileid', full_name='iteasycomm.PkgFileApprovedPush.clientfileid', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='status', full_name='iteasycomm.PkgFileApprovedPush.status', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='statusdesc', full_name='iteasycomm.PkgFileApprovedPush.statusdesc', index=3,
      number=4, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='srvapprovedmsg', full_name='iteasycomm.PkgFileApprovedPush.srvapprovedmsg', index=4,
      number=5, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='srvapprovedtime', full_name='iteasycomm.PkgFileApprovedPush.srvapprovedtime', index=5,
      number=6, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='srvapprovedusername', full_name='iteasycomm.PkgFileApprovedPush.srvapprovedusername', index=6,
      number=7, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='filemd5', full_name='iteasycomm.PkgFileApprovedPush.filemd5', index=7,
      number=8, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1796,
  serialized_end=1989,
)


_PKGFILEAPPROVEDECHO = _descriptor.Descriptor(
  name='PkgFileApprovedEcho',
  full_name='iteasycomm.PkgFileApprovedEcho',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='srvfileid', full_name='iteasycomm.PkgFileApprovedEcho.srvfileid', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='issuccess', full_name='iteasycomm.PkgFileApprovedEcho.issuccess', index=1,
      number=2, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1991,
  serialized_end=2050,
)

_STRULOGREQ.fields_by_name['logtype'].enum_type = _STRULOGREQ_LOGTYPEENUM
_STRULOGREQ.fields_by_name['loglevel'].enum_type = _STRULOGREQ_LOGLEVEL
_STRULOGREQ.fields_by_name['logcmd'].enum_type = _STRULOGREQ_LOGCMD
_STRULOGREQ_LOGTYPEENUM.containing_type = _STRULOGREQ;
_STRULOGREQ_LOGLEVEL.containing_type = _STRULOGREQ;
_STRULOGREQ_LOGCMD.containing_type = _STRULOGREQ;
_PKGLOGREQ.fields_by_name['logreqlist'].message_type = _STRULOGREQ
_PKGLOGREPLY.fields_by_name['logreplylist'].message_type = _STRULOGREPLY
DESCRIPTOR.message_types_by_name['PkgPing'] = _PKGPING
DESCRIPTOR.message_types_by_name['PkgConnectReq'] = _PKGCONNECTREQ
DESCRIPTOR.message_types_by_name['PkgConnectReply'] = _PKGCONNECTREPLY
DESCRIPTOR.message_types_by_name['PkgLoginReq'] = _PKGLOGINREQ
DESCRIPTOR.message_types_by_name['PkgLoginReply'] = _PKGLOGINREPLY
DESCRIPTOR.message_types_by_name['StruLogReq'] = _STRULOGREQ
DESCRIPTOR.message_types_by_name['PkgLogReq'] = _PKGLOGREQ
DESCRIPTOR.message_types_by_name['StruLogReply'] = _STRULOGREPLY
DESCRIPTOR.message_types_by_name['PkgLogReply'] = _PKGLOGREPLY
DESCRIPTOR.message_types_by_name['PkgPolicyPush'] = _PKGPOLICYPUSH
DESCRIPTOR.message_types_by_name['PkgPolicyEcho'] = _PKGPOLICYECHO
DESCRIPTOR.message_types_by_name['PkgPolicyinlinePush'] = _PKGPOLICYINLINEPUSH
DESCRIPTOR.message_types_by_name['PkgPolicyinlineEcho'] = _PKGPOLICYINLINEECHO
DESCRIPTOR.message_types_by_name['PkgFileApprovedPush'] = _PKGFILEAPPROVEDPUSH
DESCRIPTOR.message_types_by_name['PkgFileApprovedEcho'] = _PKGFILEAPPROVEDECHO

class PkgPing(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGPING

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgPing)

class PkgConnectReq(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGCONNECTREQ

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgConnectReq)

class PkgConnectReply(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGCONNECTREPLY

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgConnectReply)

class PkgLoginReq(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGLOGINREQ

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgLoginReq)

class PkgLoginReply(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGLOGINREPLY

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgLoginReply)

class StruLogReq(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STRULOGREQ

  # @@protoc_insertion_point(class_scope:iteasycomm.StruLogReq)

class PkgLogReq(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGLOGREQ

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgLogReq)

class StruLogReply(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STRULOGREPLY

  # @@protoc_insertion_point(class_scope:iteasycomm.StruLogReply)

class PkgLogReply(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGLOGREPLY

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgLogReply)

class PkgPolicyPush(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGPOLICYPUSH

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgPolicyPush)

class PkgPolicyEcho(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGPOLICYECHO

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgPolicyEcho)

class PkgPolicyinlinePush(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGPOLICYINLINEPUSH

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgPolicyinlinePush)

class PkgPolicyinlineEcho(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGPOLICYINLINEECHO

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgPolicyinlineEcho)

class PkgFileApprovedPush(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGFILEAPPROVEDPUSH

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgFileApprovedPush)

class PkgFileApprovedEcho(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PKGFILEAPPROVEDECHO

  # @@protoc_insertion_point(class_scope:iteasycomm.PkgFileApprovedEcho)


# @@protoc_insertion_point(module_scope)
