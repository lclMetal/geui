<map version="1.0.1">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node CREATED="1541175032501" ID="ID_1237700926" MODIFIED="1541262473881" STYLE="bubble" TEXT="GEUI">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1541175100921" ID="ID_469433986" MODIFIED="1541264497455" POSITION="right" STYLE="bubble" TEXT="item">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1541263507291" ID="ID_589856150" MODIFIED="1543074683308">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>Definition</b>
    </p>
    <p style="text-align: left">
      typedef struct WindowItemStruct
    </p>
    <p style="text-align: left">
      {
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;int index;
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;char tag[256];
    </p>
    <p style="text-align: left">
      
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;ItemType type;
    </p>
    <p style="text-align: left">
      
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;union ItemDataUnion
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;{
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;struct TextItem text;
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;struct ButtonItem button;
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;}data;
    </p>
    <p style="text-align: left">
      
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;struct WindowStruct *parent;
    </p>
    <p style="text-align: left">
      &#160;&#160;&#160;&#160;struct WindowItemStruct *next;
    </p>
    <p style="text-align: left">
      }WindowItem;
    </p>
  </body>
</html>
</richcontent>
<node CREATED="1541263684841" ID="ID_848715215" MODIFIED="1541263772750">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>title</b>
    </p>
    <p>
      struct TitleItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      }title;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1541263684841" ID="ID_876437832" MODIFIED="1541263857968">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>label</b>
    </p>
    <p>
      struct LabelItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      }label;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1541263684841" ID="ID_1610218430" MODIFIED="1541263881351">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>text</b>
    </p>
    <p>
      struct TextItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      }text;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1541263684841" ID="ID_1607036743" MODIFIED="1543074422441">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>button</b>
    </p>
    <p>
      struct ButtonItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      &#160;&#160;&#160;&#160;short state;
    </p>
    <p>
      &#160;&#160;&#160;&#160;Actor *actor;
    </p>
    <p>
      &#160;&#160;&#160;&#160;void (*actionFunction)(Window *, WindowItem *);
    </p>
    <p>
      }button;
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1541263684841" ID="ID_1413599143" MODIFIED="1541263953891">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>text field</b>
    </p>
    <p>
      struct TextFieldItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text hint;
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      }textField;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1541263684841" ID="ID_1332421366" MODIFIED="1541264331247">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>dropdown</b>
    </p>
    <p>
      struct DropdownItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      &#160;&#160;&#160;&#160;struct DropdownItemItem *items;
    </p>
    <p>
      &#160;&#160;&#160;&#160;struct DropdownItemItem *choice;
    </p>
    <p>
      }dropdown;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1541263684841" ID="ID_1744237276" MODIFIED="1541264085710">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>dropdown item</b>
    </p>
    <p>
      struct DropdownItemItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      }dropdownItem;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1541263684841" ID="ID_436969637" MODIFIED="1541264298162">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>list</b>
    </p>
    <p>
      struct ListItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      &#160;&#160;&#160;&#160;struct ListItemItem *items;
    </p>
    <p>
      &#160;&#160;&#160;&#160;struct ListItemItem *choice;
    </p>
    <p>
      }list;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1541263684841" ID="ID_1188713505" MODIFIED="1541264293734">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>list item</b>
    </p>
    <p>
      struct ListItemItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      }listItem;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1541263684841" ID="ID_1714787917" MODIFIED="1541264281219">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>menu</b>
    </p>
    <p>
      struct MenuItem
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;Text text;
    </p>
    <p>
      }menu;
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1541263556141" ID="ID_1940111096" MODIFIED="1543079487624">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>Functions </b>
    </p>
    <p style="text-align: left">
      getDataByTag(Window *win, char tag[256])
    </p>
    <p style="text-align: left">
      getDataByIndex(Window *win, int index)
    </p>
    <p style="text-align: left">
      setDataByTag(Window *win, char tag[256])
    </p>
    <p style="text-align: left">
      setDataByIndex(Window *win, int index)
    </p>
    <p style="text-align: left">
      
    </p>
    <p style="text-align: left">
      doMouseOver(const char *actorName)
    </p>
    <p style="text-align: left">
      doMouseLeave(const char *actorName)
    </p>
    <p style="text-align: left">
      doMouseButtonDown(const char *actorName, short mButton)
    </p>
    <p style="text-align: left">
      doMouseButtonUp(const char *actorName, short mButton)
    </p>
    <p style="text-align: left">
      doKeyDown(Window *win, WindowItem *item, short key)
    </p>
    <p style="text-align: left">
      doKeyUp(Window *win, WindowItem *item, short key)
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
<node CREATED="1541261430422" ID="ID_1842706526" MODIFIED="1541264496584" POSITION="left" STYLE="bubble" TEXT="window">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1541263177191" ID="ID_1833897807" MODIFIED="1543074461590">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>Definition</b>
    </p>
    <p>
      typedef struct WindowStruct
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;&#160;&#160;int index
    </p>
    <p>
      &#160;&#160;&#160;&#160;int iIndex
    </p>
    <p>
      &#160;&#160;&#160;&#160;char tag[256]
    </p>
    <p>
      &#160;&#160;&#160;&#160;Style style
    </p>
    <p>
      &#160;&#160;&#160;&#160;struct WindowItemStruct *iList
    </p>
    <p>
      &#160;&#160;&#160;&#160;struct WindowStruct *next
    </p>
    <p>
      }
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1541263269087" ID="ID_32019194" MODIFIED="1541263483120">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p style="text-align: center">
      <b>Functions</b>
    </p>
    <p>
      createWindow()
    </p>
    <p>
      destroyWindow(Window *win)
    </p>
    <p>
      openWindow(Window *win)
    </p>
    <p>
      closeWindow(Window *win)
    </p>
    <p>
      addToWindow(Window *win, WindowItem *add)
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</map>
