# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_program('dvhop-example', ['wifi', 'internet','dvhop', 'netanim'])
    obj.source = 'dvhop-example.cc'

    obj = bld.create_ns3_program('dvhop-critical', ['wifi', 'internet','dvhop', 'netanim'])
    obj.source = 'dvhop-critical.cc'