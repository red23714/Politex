import pytest

from proccess_input import ProcessInput

@pytest.fixture
def commands(tmp_path):
    from config import Config
    path = tmp_path / "config.mim"
    path.write_text(
        "[normal]\n"
        "RIGHT = go pos r 1\n"
        "x = del\n"
        "gg = go st\n"
        "dd = del line\n"
        "diw = del word space\n"
        "yy = cpy line\n"
        "@num@G = go line @num@\n",
        encoding="latin-1")
    config = Config(str(path))
    return config.commands_for_mode("normal")

def test_single_key_command(commands):
    pi = ProcessInput()
    command = pi.feed("x", commands)
    assert command is not None
    assert command.steps == [("del", [])]
    assert pi.buffer == ""

def test_sequence_needs_two_keys(commands):
    pi = ProcessInput()
    assert pi.feed("g", commands) is None
    command = pi.feed("g", commands)
    assert command is not None
    assert command.steps == [("go start", [])]

def test_prefix_keeps_buffering(commands):
    pi = ProcessInput()
    assert pi.feed("d", commands) is None
    assert pi.feed("i", commands) is None
    command = pi.feed("w", commands)
    assert command is not None
    assert command.steps == [("del word space", [])]

def test_number_then_G(commands):
    pi = ProcessInput()
    assert pi.feed("1", commands) is None
    assert pi.feed("0", commands) is None
    command = pi.feed("G", commands)
    assert command is not None
    assert command.captured == {"@num@": 10}
    assert command.steps == [("go line", ["@num@"])]

def test_unknown_sequence_resets(commands):
    pi = ProcessInput()
    assert pi.feed("d", commands) is None
    assert pi.feed("z", commands) is None
    assert pi.buffer == ""
    command = pi.feed("x", commands)
    assert command is not None

def test_match_exact_for_command_mode(commands):
    command = ProcessInput.match_exact(commands, "10G")
    assert command is not None
    assert command.captured == {"@num@": 10}

def test_can_still_match(commands):
    assert ProcessInput.can_still_match(commands, "g") is True
    assert ProcessInput.can_still_match(commands, "d") is True
    assert ProcessInput.can_still_match(commands, "dz") is False
    assert ProcessInput.can_still_match(commands, "10") is True
