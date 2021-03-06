open Oni_Core;
open Oni_Model;
open Oni_IntegrationTestLib;

module Log = (
  val Log.withNamespace("IntegrationTest.ClipboardInsertModePasteSingleLine")
);

runTest(
  ~name="InsertMode test - effects batched to runEffects",
  (dispatch, wait, runEffects) => {
  wait(~name="Initial mode is normal", (state: State.t) =>
    state.mode == Vim.Types.Normal
  );

  dispatch(KeyboardInput("i"));

  wait(~name="Mode switches to insert", (state: State.t) =>
    state.mode == Vim.Types.Insert
  );

  setClipboard(Some("def"));

  /* Simulate multiple events getting dispatched before running effects */
  dispatch(KeyboardInput("A"));
  dispatch(Command("editor.action.clipboardPasteAction"));
  dispatch(KeyboardInput("B"));

  runEffects();

  wait(~name="Buffer shows AdefB", (state: State.t) =>
    switch (Selectors.getActiveBuffer(state)) {
    | None => false
    | Some(buf) =>
      let line = Buffer.getLine(0, buf) |> BufferLine.raw;
      Log.info("Current line is: |" ++ line ++ "|");
      String.equal(line, "AdefB");
    }
  );
});
