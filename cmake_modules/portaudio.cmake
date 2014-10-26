<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>mmdagent/MMDAgent/cmake_modules/FindPortAudio.cmake in artificial_communication_agent/artificial_communication_agent:master - Gitorious</title>
    <meta http-equiv="X-UA-Compatible" content="chrome=1">
    
      <link href="/dist/bootstrap/css/bootstrap.min.css?1412681446" rel="stylesheet" type="text/css">
      <link href="/dist/bootstrap/css/bootstrap-responsive.min.css?1412681446" rel="stylesheet" type="text/css">
      <link href="/dist/css/gitorious3.min.css?1412681446" rel="stylesheet" type="text/css">
      <link href="/ui3/js/lib/jquery-ui/themes/base/minified/jquery-ui.min.css" rel="stylesheet" type="text/css">
      <link href="/assets/issues/application.css" rel="stylesheet" type="text/css">
    
    <link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
    <script type="text/javascript">
var _gaq = _gaq || [];
_gaq.push(['_setAccount', 'UA-52238-3']);
_gaq.push(['_setDomainName', '.gitorious.org'])
_gaq.push(['_trackPageview']);
(function() {
   var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
   ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
   (document.getElementsByTagName('head')[0] || document.getElementsByTagName('body')[0]).appendChild(ga);
})();
</script>
  </head>
  <body>
    <div class="page-wrapper">
      <div class="page-content">
        <div class="gts-masthead navbar">
          <div class="navbar-inner">
            <div class="container">
              
              <a href="/" class="gts-logo"><img src="/dist/images/gitorious2013.png" alt="Gitorious" title="Gitorious"></a>
              <ul class="nav">
                <li>
                  <a href="/activities">Activities</a>
                </li>
                <li>
                  <a href="/projects">Projects</a>
                </li>
                <li>
                  <a href="/groups">Teams</a>
                </li>
                <li>
                  <a href="/about">About</a>
                </li>
              </ul>
              <div class="pull-right">
                <form action="/search" class="navbar-search pull-left" method="get">
                  <input type="search" placeholder="Search" class="search-query span2" name="q">
                </form>
                <div class="pull-right btn-group login_button">
                  <a href="/login" class="btn btn-inverse">
                    <i class="icon-user icon-white"></i> Sign in
                  </a>
                </div>
              </div>
            </div>
            

          </div>
        </div>
          <div class="gts-site-header" data-gts-env-repository-view-state-path="/users/view_state/repository/59503.json">
  <div class="container">
    <div class="row">
                <h2 class="span">
            <a href="/artificial_communication_agent">artificial_communication_agent</a> /
            <a class="gts-repository-name-ph" href="/artificial_communication_agent/artificial_communication_agent">artificial_communication_agent</a>
          </h2>

    </div>
    <div class="row">
      <div class="span">
                <div class="btn-group gts-repo-urls">
          <a class="btn gts-repo-url active" href="git@gitorious.org:artificial_communication_agent/artificial_communication_agent.git">SSH</a>
          <a class="btn gts-repo-url" href="https://gitorious.org/artificial_communication_agent/artificial_communication_agent.git">HTTPS</a>
          <a class="btn gts-repo-url" href="git://gitorious.org/artificial_communication_agent/artificial_communication_agent.git">Git</a>
          <input class="span4 gts-current-repo-url gts-select-onfocus" type="url" value="git@gitorious.org:artificial_communication_agent/artificial_communication_agent.git">
          <button data-toggle="collapse" data-target="#repo-url-help" class="gts-repo-url-help btn">?</button>
        </div>

      </div>
                <div class="pull-right">
                      <a href="/artificial_communication_agent/artificial_communication_agent/archive/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3.tar.gz" class="btn gts-download" rel="tooltip" data-original-title="Download 0f18333 as .tar.gz">
            <i class="icon icon-download"></i> Download
          </a>

            <div class="gts-watch-repository-ph gts-placeholder"></div>
            <div class="gts-clone-repository-ph gts-placeholder"></div>
          </div>

    </div>
    <div class="collapse gts-help row" id="repo-url-help">
                <div class="alert alert-info span pull-right">
            <p>
              To <strong>clone</strong> this repository:
            </p>
            <pre class="prettyprint">git clone git@gitorious.org:artificial_communication_agent/artificial_communication_agent.git</pre>
            <p>
              To <strong>push</strong> to this repository:
            </p>
            <pre class="prettyprint"># Add a new remote
git remote add origin git@gitorious.org:artificial_communication_agent/artificial_communication_agent.git

# Push the master branch to the newly added origin, and configure
# this remote and branch as the default:
git push -u origin master

# From now on you can push master to the "origin" remote with:
git push</pre>
          </div>

    </div>
    <div class="row gts-description">
      <div class="span gts-repository-description-ph">
        <div class="gts-markup"><p>This project allows to enable Linux support in MMDAgent sources</p></div>
      </div>
    </div>
                <ul class="nav nav-tabs gts-header-nav gts-pjax" data-gts-active="source">
            <li class="active"><a href="/artificial_communication_agent/artificial_communication_agent/source/master:">Source code</a></li><li><a href="/artificial_communication_agent/artificial_communication_agent/activities">Activities</a></li><li><a href="/artificial_communication_agent/artificial_communication_agent/commits/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3">Commits</a></li><li><a href="/artificial_communication_agent/artificial_communication_agent/merge_requests">Merge requests <span class="count gts-mr-count-ph"></span></a></li><li><a href="/artificial_communication_agent/artificial_communication_agent/community">Community</a></li>
            <li class="gts-repository-admin-ph gts-placeholder"></li>
            <li class="gts-request-merge-ph gts-placeholder"></li>
          </ul>

  </div>
</div>

          <div class="container gts-body" id="gts-pjax-container">
  <div class="gts-file gts-browser">
    <div class="gts-ref-selector-ph"></div>
    <ul class="pull-right gts-blob-view">
      <li class="active">Blob content</li>
      <li><a href="/artificial_communication_agent/artificial_communication_agent/blame/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3:mmdagent/MMDAgent/cmake_modules/FindPortAudio.cmake">Blame</a></li>
      <li><a href="/artificial_communication_agent/artificial_communication_agent/history/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3:mmdagent/MMDAgent/cmake_modules/FindPortAudio.cmake">History</a></li>
      <li><a href="/artificial_communication_agent/artificial_communication_agent/raw/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3:mmdagent/MMDAgent/cmake_modules/FindPortAudio.cmake">Raw blob</a></li>
    </ul>
              <ul class="breadcrumb">
            <li><a href="/artificial_communication_agent/artificial_communication_agent/source/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3:"><i class="icon icon-file"></i> /</a></li>
            <li><a href="/artificial_communication_agent/artificial_communication_agent/source/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3:mmdagent">mmdagent<span class="divider">/</span></a></li><li><a href="/artificial_communication_agent/artificial_communication_agent/source/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3:mmdagent/MMDAgent">MMDAgent<span class="divider">/</span></a></li><li><a href="/artificial_communication_agent/artificial_communication_agent/source/0f18333cb3f6308e1d272e6c3ec33eb5b1db66e3:mmdagent/MMDAgent/cmake_modules">cmake_modules<span class="divider">/</span></a></li><li class="active">FindPortAudio.cmake</li>
          </ul>

    <pre class="cmake prettyprint linenums"><ol class="linenums gts-lines"><li class="L1"><span class="line"><span class="c"># Try to find PortAudio</span></span></li><li class="L2"><span class="line"><span class="c"># Once done, this will define</span></span></li><li class="L3"><span class="line"><span class="err">#</span></span></li><li class="L4"><span class="line"><span class="c"># PORTAUDIO_FOUND - system has PortAudio</span></span></li><li class="L5"><span class="line"><span class="c"># PORTAUDIO_INCLUDE_DIR - the PortAudio include directories</span></span></li><li class="L6"><span class="line"><span class="c"># PORTAUDIO_LIBRARIES - link these to use PortAudio</span></span></li><li class="L7"><span class="line"> </span></li><li class="L8"><span class="line"><span class="nb">if</span><span class="p">(</span><span class="s">PORTAUDIO_INCLUDE_DIR</span> <span class="s">AND</span> <span class="s">PORTAUDIO_LIBRARIES</span><span class="p">)</span></span></li><li class="L9"><span class="line">    <span class="nb">set</span><span class="p">(</span><span class="s">PORTAUDIO_FIND_QUIETLY</span> <span class="s">TRUE</span><span class="p">)</span></span></li><li class="L10"><span class="line"><span class="nb">endif</span><span class="p">(</span><span class="s">PORTAUDIO_INCLUDE_DIR</span> <span class="s">AND</span> <span class="s">PORTAUDIO_LIBRARIES</span><span class="p">)</span></span></li><li class="L11"><span class="line"> </span></li><li class="L12"><span class="line"><span class="c"># include dir</span></span></li><li class="L13"><span class="line"><span class="nb">find_path</span><span class="p">(</span><span class="s">PORTAUDIO_INCLUDE_DIR</span> <span class="s">portaudio.h</span><span class="p">)</span></span></li><li class="L14"><span class="line"> </span></li><li class="L15"><span class="line"><span class="c"># finally the library itself</span></span></li><li class="L16"><span class="line"><span class="nb">find_library</span><span class="p">(</span><span class="s">libPortAudio</span> <span class="s">NAMES</span> <span class="s">portaudio</span><span class="p">)</span></span></li><li class="L17"><span class="line"><span class="c"># Seems to run OK without libportaudiocpp so do not pull additional dependency</span></span></li><li class="L18"><span class="line"><span class="c"># find_library(libPortAudioCpp NAMES portaudiocpp)</span></span></li><li class="L19"><span class="line"><span class="c"># set(PORTAUDIO_LIBRARIES ${libPortAudio} ${libPortAudioCpp})</span></span></li><li class="L20"><span class="line"><span class="nb">set</span><span class="p">(</span><span class="s">PORTAUDIO_LIBRARIES</span> <span class="o">${</span><span class="nv">libPortAudio</span><span class="o">}</span><span class="p">)</span></span></li><li class="L21"><span class="line"> </span></li><li class="L22"><span class="line"><span class="c"># handle the QUIETLY and REQUIRED arguments and set PORTAUDIO_FOUND to TRUE if </span></span></li><li class="L23"><span class="line"><span class="c"># all listed variables are TRUE</span></span></li><li class="L24"><span class="line"><span class="nb">include</span><span class="p">(</span><span class="s">FindPackageHandleStandardArgs</span><span class="p">)</span></span></li><li class="L25"><span class="line"><span class="nb">find_package_handle_standard_args</span><span class="p">(</span><span class="s">PortAudio</span> <span class="s">DEFAULT_MSG</span> <span class="s">PORTAUDIO_LIBRARIES</span> <span class="s">PORTAUDIO_INCLUDE_DIR</span><span class="p">)</span></span></li><li class="L26"><span class="line"> </span></li><li class="L27"><span class="line"><span class="nb">mark_as_advanced</span><span class="p">(</span><span class="s">PORTAUDIO_LIBRARIES</span> <span class="s">PORTAUDIO_INCLUDE_DIR</span><span class="p">)</span></span></li></ol></pre>
  </div>
</div>

      </div>
    </div>
    <div class="footer">
      
<div class="container">
  <p class="pull-right gts-powered-by">
    <a href="http://getgitorious.com">
      <img alt="Powered by Gitorious" src="/assets/poweredby.png" title="Powered by Gitorious">
    </a>
  </p>
  <div class="row-fluid">
      <div class="span3">
        <ul>
          <li><a href="/about">About Gitorious</a></li>
          <li><a href="http://groups.google.com/group/gitorious">Discussion group</a></li>
          <li><a href="http://blog.gitorious.org">Blog</a></li>
        </ul>
      </div>
      <div class="span3">
        <ul>
          <li><a href="http://en.gitorious.org/tos">Terms of Service</a></li>
          <li><a href="http://en.gitorious.org/privacy_policy">Privacy Policy</a></li>
          <li><a href="http://gitorious.com">Professional Gitorious Services</a></li>
        </ul>
      </div>
      <div class="span3">
        <ul>
          <li><a href="http://en.gitorious.org/security">Security</a></li>
        </ul>
      </div>
  </div>
  <p>
    Gitorious is free software licensed under
    the <a href="http://www.gnu.org/licenses/agpl-3.0.html">GNU Affero
      General Public License (AGPL)</a> unless noted otherwise.
    GIT is a trademark of Software Freedom Conservancy and Gitorious' use of
    "GIT" is under license.
  </p>
  <p class="gts-com-plug">
    <a href="http://gitorious.com">Professional Gitorious services</a> - Git
    hosting at your company, custom features, support and more.
    <a href="http://gitorious.com">gitorious.com</a>.
  </p>
</div>

    </div>
    <script data-gts-env-user-view-state-path="/users/view_state.json"></script>
      <script
        data-gts-env-repository-refs-url="/artificial_communication_agent/artificial_communication_agent/refs"
        data-gts-env-commit-url-template="/artificial_communication_agent/artificial_communication_agent/commit/#{oid}">
      </script>
      <script src="/dist/js/gitorious3.min.js?1412681446"></script>
      <script src="/ui3/js/lib/jquery-ui/ui/jquery.ui.menu.js"></script>
      <script src="/ui3/js/lib/jquery-ui/ui/jquery.ui.autocomplete.js"></script>
      <script src="/ui3/js/lib/jquery-ui/ui/jquery.ui.datepicker.js"></script>
      <script src="/assets/issues/application.js"></script>
    
    
    
    <script>
      $(function() { $('.alert').append('<a class="close" data-dismiss="alert" href="#">&times;</a>').alert(); });
      gts.enableJS(document.documentElement);
    </script>
  </body>
</html>
