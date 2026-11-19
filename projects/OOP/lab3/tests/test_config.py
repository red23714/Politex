import pytest

from config import Config

@pytest.fixture
def config(tmp_path):
    path = tmp_path / "config.mim"
    path.write_text(
        "[normal]\n"
        "RIGHT = go pos r 1\n"
        "gg = go st\n"
        "@num@G = go line @num@\n"
        "dd = del line\n"
        "S = del line insert\n"
        "i = insert bf\n"
        "/@text@ = search @text@ st\n"
        "[command]\n"
        "o @text@ = open @text@\n"
        "q! = exit force\n"
        "wq! = save exit\n"
        "set num = numbering toggle\n",
        encoding="latin-1")
    return Config(str(path))

def test_parses_sections(config):
    normal = config.commands_for_mode("normal")
    command = config.commands_for_mode("command")
    assert len(normal) == 7
    assert len(command) == 4

def test_simple_navigation(config):
    right = config.commands_for_mode("normal")[0]
    assert right.keys == "RIGHT"
    assert right.steps == [("go pos right", [1])]
    assert right.is_sequence is False

def test_sequence_keys(config):
    commands = {c.keys: c for c in config.commands_for_mode("normal")}
    gg = commands["gg"]
    assert gg.is_sequence is True
    assert gg.steps == [("go start", [])]

def test_placeholder_num(config):
    commands = {c.keys: c for c in config.commands_for_mode("normal")}
    ng = commands["@num@G"]
    assert ng.steps == [("go line", ["@num@"])]

def test_compound_command_split_by_verbs(config):
    commands = {c.keys: c for c in config.commands_for_mode("normal")}
    s = commands["S"]
    assert s.steps == [("del line", []), ("insert", [])]

def test_search_placeholder(config):
    commands = {c.keys: c for c in config.commands_for_mode("normal")}
    search = commands["/@text@"]
    assert search.steps == [("search start", ["@text@"])]

def test_command_mode_aliases(config):
    commands = {c.keys: c for c in config.commands_for_mode("command")}
    assert commands["o @text@"].steps == [("open", ["@text@"])]
    assert commands["q!"].steps == [("exit force", [])]
    assert commands["wq!"].steps == [("save", []), ("exit", [])]
    assert commands["set num"].steps == [("numbering toggle", [])]

def test_invalid_lines_are_skipped(tmp_path):
    path = tmp_path / "bad.mim"
    path.write_text("garbage line without equals\n\n= empty keys\n",
                    encoding="latin-1")
    config = Config(str(path))
    assert config.commands == []
