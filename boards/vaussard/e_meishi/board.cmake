board_runner_args(jlink "--device=STM32U375CE" "--reset-after-load")

include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
