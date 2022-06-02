# ROOT Data Analysis Framework

Some Document references:
* https://root.cern/about/
* https://root.cern/install/
* https://root.cern/install/build_from_source/
  * note the drop-down 'build options for ROOT 6.24' with build variables
* https://root.cern/cling/
* https://root.cern/manual/first_steps_with_root/
* https://root.cern/manual/integrate_root_into_my_cmake_project/
* https://root.cern/manual/creating_a_user_application/
* https://root-forum.cern.ch/t/how-to-embed-a-tcanvas-in-external-applications/28247
* https://root.cern/manual/embed_root_in_gui/
* https://root.cern/doc/master/index.html
* https://root.cern/doc/master/classTRint.html
* https://root.cern/root/htmldoc/guides/users-guide/Threads.html
* https://root.cern/manual/multi_threading/

* https://root.cern/root/htmldoc/guides/users-guide/

* Tutorial
** https://agnieszkamucha.github.io/ParticlePhysics/Files/Tutorial-ROOT.pdf

Some contrary comments on ROOT (from the cint days, not the recent cling):
* http://insectnation.org/articles/basic-root.html
* http://insectnation.org/articles/problems-with-root.html

# TMacro association of structures created elsewhere:

```
  TList* pList = gDirectory->GetList();
  for( const auto&& obj: *pList ) {

    TClass* class_ = (TClass*) obj;

    //class_->Dump();

    //std::cout
    //  << "name=" << class_->GetName()
    //  << ",title=" << class_->GetTitle()
    //  << ",class" << class_->ClassName()
    //  << std::endl;

    if ( 0 == strcmp( "quotes", class_->GetName() ) ) {
      pTreeQuotes = (TTree*)obj;
    }

    if ( 0 == strcmp( "trades", class_->GetName() ) ) {
      pTreeTrades = (TTree*)obj;
    }

    if ( 0 == strcmp( "h2", class_->GetName() ) ) {
      pHisto2 = (TH2F*)obj;
    }

  }
```

The following can be used to examine data offline - it invokes the browser:

```
$ /usr/local/bin/rdaf/root
root [0] TBrowser b
(TBrowser &) Name: Browser Title: ROOT Object Browser
```
