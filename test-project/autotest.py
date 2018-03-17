#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import difflib
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
		m[r[0]] = r[1:]
	return m

ok = []
failed = {}
def test_file(filename, result):
	filename2 = filename+'.should'
	with open(filename2, 'r') as should_f:
		should = should_f.read().split('\n')
	diff = difflib.unified_diff(should, result, fromfile=filename2, tofile=filename)
	diff = '\n'.join(diff).replace('\n\n','\n')
	if diff:
		failed[filename] = diff
	else:
		ok.append(filename)


try:
	bd = 'build-autotest-Debug'
	go('mkdir -p '+bd)
	go('cd '+bd)
	go('cmake ..')
	go('cmake --build .')
	g = ' '.join(glob('../sep/**/*.cpp', recursive=True))
	marked = go('marker ' + g)
	result = list_to_dict( marked.split('--8<--\n') )
	for fname, res in result.items():
		test_file(fname, res)

	for key, value in failed.items():
		print(value)


	print('Collected results for {} files'.format(len(result)))
	print('{} look like expected'.format(len(ok)))
	for o in ok:
		print(o)
	print('{} contain diffs'.format(len(failed)))
	for f in failed.keys():
		print(f)
	sys.exit(len(failed))

except Exception as e:
	print(e)
	sys.exit(1)
