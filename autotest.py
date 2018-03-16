#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
from glob import glob
from subprocess import Popen, PIPE

def go(args):
	print('$', args)
	if type(args) is str:
		args = args.split(' ')
	if (args[0] == 'cd'):
		os.chdir(args[1])
		return

	process = Popen(args, stdout=PIPE)
	(output, err) = process.communicate()
	exit_code = process.wait()
	if exit_code:
		raise Exception(err)
	return output.decode('UTF-8')

def list_to_dict(l):
	m = {}
	for listing in l:
		r = listing.split('\n')
		m[r[0]] = '\n'.join(r[1:])
	return m

try:
	bd = 'build-autotest-Debug'
	go('mkdir -p '+bd)
	go('cd '+bd)
	go('cmake ../test-project')
	go('cmake --build .')
	g = ' '.join(glob('../test-project/*.cpp'))
	marked = go('marker ' + g)
	result = list_to_dict( marked.split('--8<--\n') )
	for fname, res in result.items():
		print("file:", fname)
		print("result:",res)
		print('-'*80)

except Exception as e:
	print(e)
	sys.exit(1)
