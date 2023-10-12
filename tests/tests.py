import pytest
import wget
import os

@pytest.fixture()
def strategy(target):
    try:
        target.get_driver('SerialDriver')
        return target.get_driver('UBootStrategy')
    except NoDriverFoundError:
        pytest.skip("strategy not found")

@pytest.fixture(scope="function")
def in_bootloader(strategy, capsys):
    with capsys.disabled():
        strategy.transition("uboot")

@pytest.fixture(scope="function")
def in_shell(strategy, capsys):
    with capsys.disabled():
        strategy.transition("shell")

def test_uboot(target, in_bootloader):
    command = target.get_driver('UBootDriver')
    stdout, stderr, returncode = command.run('version')
    assert returncode == 0
    assert stdout
    assert not stderr
    assert 'U-Boot' in '\n'.join(stdout)

def test_shell(target, in_shell):
    command = target.get_driver('ShellDriver')
    stdout, stderr, returncode = command.run('cat /proc/version')
    assert returncode == 0
    assert stdout
    assert not stderr
    assert 'Linux' in stdout[0]
