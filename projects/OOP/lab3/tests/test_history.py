import pytest

from command import Command
from history import History

def test_singleton_identity():
    assert History() is History()

def test_push_pop_order():
    history = History()
    history.clear()

    first = Command("dd", True, [("del line", [])])
    second = Command("x", False, [("del", [])])
    history.push(first, ("snap1",))
    history.push(second, ("snap2",))

    command, snapshot = history.pop()
    assert command is second
    assert snapshot == ("snap2",)
    command, snapshot = history.pop()
    assert command is first
    assert history.pop() is None

def test_describe():
    history = History()
    history.clear()
    command = Command("10G", True, [("go line", ["@num@"])],
                      captured={"@num@": 10})
    history.push(command, ())
    assert history.describe() == ["10G -> go line [10]"]

def test_clear_and_limit():
    history = History(limit=3)
    history.clear()
    for index in range(5):
        history.push(Command(f"k{index}", False, []), (index,))
    assert len(history) == 3
    history.clear()
    assert len(history) == 0
