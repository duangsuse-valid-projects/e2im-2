package org.duangsuse

typealias Cnt = Int
typealias Ordinal = Int
typealias Path = String

typealias ConsumerOn<T, A1> = T.(A1) -> Unit
typealias Predicate<T> = (T) -> Boolean
val never: Predicate<Any?> get() = { false }

/** An abstract random-bulk-access sequence of boolean flag */
interface ArrayFlags<T> {
  fun checkFlags(vararg item: T): BooleanArray
  @Throws(Fail::class)
  fun setFlags(vararg item: T)
  @Throws(Fail::class)
  fun unsetFlags(vararg item: T)
  data class Fail(val position: Cnt, val errno: Ordinal, val newState: Boolean?):
    Error("Failed at item #$position (err $errno)${newState.prefixElvis("→ ")}") {
  }
}

private fun <T: Any> T?.prefixElvis(prefix: String): String = this?.let { prefix+it } ?: ""

/** Try [op] for [initial]. when it fails with [ArrayFlags.Fail],
 *  continue call [op] with failed position when not [terminate] */
fun <T> ArrayFlags<T>.retry(terminate: Predicate<ArrayFlags.Fail> = never,
    initial: Array<T>, op: ConsumerOn<ArrayFlags<T>, Array<T>>) {
  var rest = initial
  while (true) try { this.op(rest); break; }
  catch (ff: ArrayFlags.Fail) {
    if (terminate(ff)) return
    val (pos, _) = ff
    rest = rest.sliceArray(pos..rest.lastIndex)
  }
}
