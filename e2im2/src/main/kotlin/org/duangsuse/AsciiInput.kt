package org.duangsuse

import java.io.InputStream

/** Unsafe ([InputStream.read] `-1` return value is discard) ASCII(8-bit) input */
class AsciiInput(private val inner: InputStream): CharIterator() {
  override fun hasNext(): Boolean = inner.available() != 0
  override fun nextChar(): Char = inner.read().toChar()
}
