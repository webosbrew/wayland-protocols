#!/usr/bin/env python3
from pathlib import Path
from typing import Set, Optional, List, Literal
from xml.dom import minidom
from xml.dom.minidom import Document, Element

protos_dir = Path(__file__).parent
dumped_dir = protos_dir.joinpath('dumped')

templates_dir = protos_dir.joinpath('templates')


def load_interfaces(p: Path) -> List[Element]:
    doc: Document = minidom.parse(str(p))
    return doc.documentElement.getElementsByTagName('interface')


def signature_matches(request_a: Element, request_b: Element) -> bool:
    args_a = request_a.getElementsByTagName('arg')
    args_b = request_b.getElementsByTagName('arg')
    if len(args_a) != len(args_b):
        return False
    for arg_a, arg_b in zip(args_a, args_b):
        if arg_a.getAttribute('type') != arg_b.getAttribute('type'):
            return False
    return True


for ver_dir in dumped_dir.iterdir():
    ver_interfaces: Set[Element] = set(load_interfaces(ver_dir.joinpath('webos.xml')))
    merged_dir = protos_dir.joinpath('merged', ver_dir.name)
    merged_dir.mkdir(exist_ok=True)


    def pop_interface(name: str) -> Optional[Element]:
        item = next(filter(lambda e: e.getAttribute('name') == name, ver_interfaces), None)
        if not item:
            return None
        ver_interfaces.remove(item)
        return item


    def gen_protocol(template: Element) -> Optional[Document]:
        doc_out = Document()
        root_out = doc_out.createElement('protocol')
        root_out.setAttribute('name', template.getAttribute('name'))
        doc_out.appendChild(root_out)
        has_interface: bool = False
        for interface in template.getElementsByTagName('interface'):
            def find_message(name: str, msg_type: Literal['request'] or Literal['event']) -> Optional[Element]:
                return next(filter(lambda e: e.getAttribute('name') == name, interface.getElementsByTagName(msg_type)),
                            None)

            if_name = interface.getAttribute('name')
            if_tv = pop_interface(if_name)
            if not if_tv:
                continue
            if_added: Element = root_out.appendChild(doc_out.importNode(if_tv, True))

            def populate_messages(msg_type: Literal['request'] or Literal['event']):
                message: Element
                for message in if_added.getElementsByTagName(msg_type):
                    msg_name = message.getAttribute('name')
                    msg_template = find_message(msg_name, msg_type)
                    if not msg_template:
                        continue
                    args: List[Element] = message.getElementsByTagName('arg')
                    args_template: List[Element] = msg_template.getElementsByTagName('arg')
                    if not signature_matches(message, msg_template):
                        continue
                    for arg, arg_template in zip(args, args_template):
                        arg.setAttribute('name', arg_template.getAttribute('name'))

            populate_messages('request')
            populate_messages('event')

            has_interface = True
        if not has_interface:
            return None
        return doc_out


    for template_path in templates_dir.glob('*.xml'):
        doc_ose: Document = minidom.parse(str(template_path))

        out = gen_protocol(doc_ose.documentElement)
        if not out:
            continue
        with merged_dir.joinpath(template_path.name).open('w') as f:
            xml: bytes = out.toprettyxml(indent='  ', newl='\n', encoding='UTF-8')
            for line in xml.splitlines(keepends=True):
                if not line.strip():
                    continue
                f.write(str(line, encoding='UTF-8'))

    for ver_interface in ver_interfaces:
        print(f'Interface {ver_interface.getAttribute("name")} unhandled in {ver_dir.name}')
