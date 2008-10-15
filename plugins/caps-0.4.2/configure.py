#! /usr/bin/env python
import os

CFLAGS = []

def we_have_sse():
	try: return 'sse' in open ('/proc/cpuinfo').read().split()
	except: return 0
def we_have_ssse3():
	try: return 'ssse3' in open ('/proc/cpuinfo').read().split()
	except: return 0

def store():
	f = open ('defines.make', 'w')
	print >> f, "_CFLAGS=" + ' '.join (CFLAGS)

if __name__ == '__main__':
	if we_have_sse():
		CFLAGS += ('-msse', '-mfpmath=sse')
	if we_have_ssse3():
		CFLAGS += ('-msse3',)
	store()
