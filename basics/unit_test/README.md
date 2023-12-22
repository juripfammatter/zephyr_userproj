# Unit test example

To run on target use `-b <target_name>` instead of `qemu_xtensa` and `west flash` instead of `west build -t run``

**Build regularly**:
* `west build -b qemu_xtensa -p always zephyr_userproj/basics/unit_test`
* `west build -t run`
<br><br>

**Build tests**:
* `west build -b qemu_xtensa -p always zephyr_userproj/basics/unit_test -DEXTRA_CONF_FILE=test.conf -DTESTS=ON`
* `west build -t run`