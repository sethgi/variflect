import argparse
import clang.cindex
from typing import Union, List, Dict
import logging

LOG_LEVEL=logging.DEBUG

VARIFLECT_TYPES = ["Variflect"]


class Variflect:
    def __init__(self, sources: Union[str, List[str]] = None) -> None:
        if isinstance(sources, str):
            self._sources = [sources]
        elif isinstance(sources, List):
            self._sources = sources
        else:
            self._sources = []

        self._index = clang.cindex.Index.create()

        self._logger = logging.Logger("Variflect", level=LOG_LEVEL)

    def add_source(self, source: str) -> None:
        self._sources.append(source)

    def parse(self):
        types = self.get_variflect_types()
        ast = self.build_ast(types)

        return types

    def build_ast(self, types: Dict[str, clang.cindex.Cursor]):
        ast = {}

        for typename, cursor in types.items():
            fields = filter(lambda x: x.kind == clang.cindex.CursorKind.FIELD_DECL, cursor.get_children())
            fields = list(fields)

            
            
    def get_variflect_types(self) -> Dict[str, clang.cindex.Cursor]:
        types = {}
        for source in self._sources:
            print(f"Processing source file {source}")
            translation_unit = self._index.parse("../include/variflect.hh")
            translation_unit = self._index.parse(source,
                                                 args=["-std=c++20", "-I../include/", "-I/usr/include/c++/12/"])

            for cursor in translation_unit.cursor.walk_preorder():
                try:
                    kind = cursor.kind
                except ValueError:
                    continue
                    
                if kind in [clang.cindex.CursorKind.CLASS_DECL,
                                clang.cindex.CursorKind.STRUCT_DECL,
                                clang.cindex.CursorKind.CLASS_TEMPLATE]:
                    for child in cursor.get_children():
                        if child.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER and child.spelling in VARIFLECT_TYPES:
                            name = cursor.spelling
                            if name in types:
                                raise RuntimeError(f"Duplicate Type found: {name}")
                            types[name] = cursor
                            break
        return types

if __name__ == "__main__":

    parser = argparse.ArgumentParser("variflect code generation")
    parser.add_argument("sources", nargs="*", type=str, help="Files to parse")
    args = parser.parse_args()


    sources = args.sources

    if len(sources) == 0:
        print("Variflect: Nothing to do")
        exit()


    variflect = Variflect(sources)
    types = variflect.parse()
    