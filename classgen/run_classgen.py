"""Run a class generator and write its output as UTF-8.

CMake's execute_process decodes a child's stdout using the host code page
(cp936 on a Chinese Windows), so a Chinese banner printed by a generator gets
mangled before file(WRITE) sees it. This wrapper keeps the text inside Python,
where the encoding is explicit, and writes the .inl itself.

Usage: run_classgen.py <generator.py> <output.inl>
"""
import io
import os
import runpy
import subprocess
import sys


def main():
    if len(sys.argv) != 3:
        sys.stderr.write("usage: run_classgen.py <generator.py> <output.inl>\n")
        return 2

    generator, output = sys.argv[1], sys.argv[2]

    env = dict(os.environ)
    env["PYTHONIOENCODING"] = "utf-8"

    proc = subprocess.run(
        [sys.executable, generator],
        cwd=os.path.dirname(os.path.abspath(generator)) or ".",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env,
    )

    if proc.returncode != 0:
        sys.stderr.write(proc.stderr.decode("utf-8", "replace"))
        return proc.returncode

    text = proc.stdout.decode("utf-8", "replace")

    with io.open(output, "w", encoding="utf-8", newline="") as f:
        f.write(text)

    return 0


if __name__ == "__main__":
    sys.exit(main())
