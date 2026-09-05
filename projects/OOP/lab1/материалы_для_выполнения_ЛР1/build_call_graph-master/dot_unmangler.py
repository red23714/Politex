import pydot
import sys, os
import argparse
import subprocess

from typing import List, Dict

CXX_FILT_PATH = "llvm-cxxfilt"

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()

    parser.add_argument('-p', '--path', help="Path to dot file", required=True, type=str)
    parser.add_argument('-o', '--out', help="Path to output file", required=False, type=str)
    parser.add_argument('--cxxfilt', help="Override path to llvm-cxxfilt utility", required=False, type=str)

    return parser.parse_args()

def unmangle_names(names: List[str], cxx_filt_path: str | None) -> Dict[str, str] | None:
    preprocessed_names = [n.strip('"').strip('{}') for n in names]
    output = subprocess.check_output([cxx_filt_path if cxx_filt_path is not None else CXX_FILT_PATH] + preprocessed_names)
    unmangled_names = output.decode('utf-8').split('\n')[:-1]
    if len(names) != len(unmangled_names):
        return None
    result: Dict[str, str] = {}
    for i in range(len(names)):
        result[names[i]] = unmangled_names[i].replace("(", "_").replace(")", "_").replace(":", "_").replace("<", "&lt").replace(">", "&ge")
    return result

def main() -> int:
    args = parse_args()

    if not os.path.isfile(args.path):
        print(f"[!] Invalid input file path ({args.path})")
        return 1

    try:
        graph = pydot.graph_from_dot_file(args.path)[0]
    except:
        print(f"[!] Invalid file format (exptecte graphviz .dot file)")
        return 1

    node_labels = [n.get_label() for n in graph.get_nodes()]
    try:
        unmangled = unmangle_names(node_labels, args.cxxfilt)
    except FileNotFoundError:
        print(f"[!] llvm-cxxfilt util was not found")
        return 1

    if unmangled is None:
        print(f"[!] Invalid output was received from llvm-cxxfilt")
        return 1

    for node in graph.get_nodes():
        label = node.get_label()
        unmangled_label = unmangled[label]
        node.set_label(unmangled_label)

    output_path = args.out if args.out is not None else f"{args.path}.unamngled.dot"
    graph.write_dot(output_path)
    print(f"[+] Saved unmangeled graph to '{output_path}'")
    return 0

if __name__ == "__main__":
    sys.exit(main())
