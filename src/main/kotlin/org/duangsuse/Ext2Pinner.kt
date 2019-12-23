package org.duangsuse

import android.annotation.TargetApi
import android.content.Context
import android.os.Build
import java.io.File.separator as sep
import java.io.PrintStream

interface FilePin: ArrayFlags<Path>
private val whitespaceEscape = mapOf(' ' to ' ', '\n' to '\n')
private fun String.escape(translate: Map<Char, Char>, escapeChar: Char = '\\'): String = fold(StringBuilder())
  { sb: StringBuilder, c: Char -> if (c in translate) sb.append(escapeChar).append(translate[c]) else sb.append(c) }.toString()

/**
 * Ext2 filesystem __'immutable' attribute__ change utility
 *
 * This class __executes, then connects__ with _sudo native program_
 * > _$_ _[commandSudo]_ _[commandPinner]_
 *
 * in Android application shared library dir [pinnerPath].
 *
 * ## Call protocol (`Pipe`)
 * > `{a}` means (repeat `a`); `(a b c)` means (sequence of `a b c`); `(a|b|c)` means (`a` or `b` or `c`); `a!t` means
 *   (ensure `t` unread before read `a`)
 * + newline `'\n'`
 * + Ws `{' '|newline}`
 * + Pipe `{Operation newline}`
 * + Operation `Action Count Ws Files`
 * + Action `'+'|'-'|'?'`
 * + Count `{digit}`
 * + digit `[0-9]`
 * + Files `{Path ws!newline}`
 * + Path `(Escape|anyChar) until ws`
 * + Escape `'\\' anyChar`
 *
 * ## Feedback protocol
 * ### `'+'`, `'-'` operation result
 * for each file input,
 * + `'.'` normal
 * + `'!' errorDigit` fail
 * ### `'?'` operation result
 * for each file input,
 * + `'t'` immutable flag set
 * + `'f'` immutable flag unset
 * + `'!' errorDigit` fail
 */
class Ext2Pinner(subProcess: Process): FilePin {
  constructor(ctx: Context): this(ProcessBuilder(*commandSudo, pinnerPath(ctx)).start())

  private val subIn = subProcess.outputStream.let(::PrintStream)
  private val subOut = subProcess.inputStream.let(::AsciiInput)

  override fun checkFlags(vararg item: Path): BooleanArray {
    writeOperation(Action.Query, item)
    return handleFlagCheckOut(item.size)
  }
  private fun handleFlagCheckOut(n: Cnt): BooleanArray {
    val flags = BooleanArray(n)
    readNextRes@ for (pos in 0 until n) {
      check(subOut.hasNext())
      when (subOut.nextChar()) {
        't' -> { flags[pos] = true }
        'f' -> { flags[pos] = false }
        '!' -> throw ArrayFlags.Fail(pos, subOut.nextChar().toInt(), null)
        else -> continue@readNextRes
      }
    }
    return flags
  }

  override fun setFlags(vararg item: Path) {
    writeOperation(Action.Set, item)
    handleFlagChangeOut(item.size, true)
  }
  override fun unsetFlags(vararg item: Path) {
    writeOperation(Action.Unset, item)
    handleFlagChangeOut(item.size, false)
  }
  private fun handleFlagChangeOut(n: Cnt, new_state: Boolean) {
    readNextRes@ for (pos in 0 until n) {
      check(subOut.hasNext())
      when (subOut.nextChar()) {
        '.' -> continue@readNextRes
        '!' -> throw ArrayFlags.Fail(pos, subOut.nextChar().toInt(), new_state)
        else -> continue@readNextRes
      }
    }
  }

  enum class Action(val cmd: Char) { Set('+'), Unset('-'), Query('?') }
  private fun writeOperation(action: Action, items: Array<out Path>) {
    fun writeAction(action: Action) { subIn.print(action.cmd) }
    fun writeCount(count: Int) { subIn.print(count.toString()) }
    fun writeWs() { subIn.print(' ') }
    fun writeFiles(files: Array<out Path>)
      { subIn.print(files.joinToString(" ") { it.escape(whitespaceEscape) }) }

    writeAction(action)
    writeCount(items.size)
    writeWs()
    writeFiles(items)
    subIn.println()
  }

  companion object {
    private val commandSudo = arrayOf("su", "-c")
    private const val commandPinner = "libfilepin.so"
    @TargetApi(Build.VERSION_CODES.GINGERBREAD)
    fun pinnerPath(ctx: Context): Path = ctx.applicationInfo.nativeLibraryDir +sep+ commandPinner
  }
}
