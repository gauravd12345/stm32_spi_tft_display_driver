/* Memory as specified in the datasheet*/
MEMORY {
  flash(rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  sram(rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}