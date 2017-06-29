_caret = None

def get_caret():
    global _caret
    return _caret

def drop():
    global _caret
    _caret = None

def acquire( ui_area ):
    global _caret
    if _caret is not None:
        _caret.revert_edit()
    _caret = ui_area

