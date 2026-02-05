import os
import subprocess
import sys

script_dir: str = os.path.dirname(os.path.abspath(__file__))
project_dir: str = os.path.abspath(os.path.join(script_dir, '..'))

def main():
	cmake_configure = ['cmake', '--preset=default']
	cmake_build = ['cmake', '--build', 'build', '--config', 'Release']

	try:
		print('⚙️ Configuring...')
		subprocess.check_call(cmake_configure, cwd=project_dir)

		print('🔨 Building...')
		subprocess.check_call(cmake_build, cwd=project_dir)

		print('✅ Success building')
	except subprocess.CalledProcessError as e:
		print(f'❌ Error building: {e}')
		sys.exit(1)

if __name__ == '__main__':
	main()
