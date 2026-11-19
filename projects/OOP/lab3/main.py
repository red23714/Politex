import sys

from app import build_app


if __name__ == "__main__":
    editor = build_app(sys.argv[1] if len(sys.argv) > 1 else None)
    editor.run()
