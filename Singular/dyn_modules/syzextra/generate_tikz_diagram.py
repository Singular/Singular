#!/usr/bin/python

import re
import argparse
import simplejson
from jinja2 import Environment
env = Environment()


#output_node_tmpl = env.from_string(r"""node[bag] { {{ '$%s$' % nodelabel if nodelabel }} {%- if noderesult %} \mathfootnote{ {{ nodelabel }} \mapsto {{ noderesult }} } {%- endif %} }""")
#output_top_node_tmpl = env.from_string(r"""\node[bag,draw] {${{ nodelabel }}$} \mathfootnote{  {{ nodelabel }} \mapsto {%- if noderesult %} {{ noderesult }}  {%- endif %} }""")
#def output_node(data, top):
#    #print data['nodename']
#    if top:
#        return output_top_node_tmpl.render(**data)
#    else:
#        return output_node_tmpl.render(**data)




# {{ output_node(self, false) }}
mypic_node_tmpl = env.from_string(r"""node[bag] {{ '(%d)' % hash(self) }}
{
  {{ '$%s$' % nodelabel if nodelabel }}  \mathfootnote{  {{ ' %s: ' % proc if proc }}  {{ ' %s ' % nodelabel if nodelabel }}  {{ ' \mapsto %s ' % noderesult if noderesult }}  {{ ' (rescale: %s) ' % rescale if rescale }}  {{ ' (look\_up\_poly: %s) ' % look_up_poly if look_up_poly }}  }
}
{%- if children %}{%- for child_node in children %}
child
{
{{ render_node(child_node) | indent(2,true) }}
}
{%- endfor %}{%- endif %}
edge from parent[ed] node[arr] {    {{ '\mathtiny{%s}' % edgelabel if edgelabel }}   }
""")


# name=%d,% hash(self)
node_tmpl = env.from_string(r"""[ {\mathfootnote{ {{ '%s' % nodelabel if nodelabel }} }{ {{'%s' % proc if proc}}{{',\lambda: %s' % recale if recale}}{{',LUP(%s)' % Lookup if Lookup}}{{'\mapsto %s' % noderesult if noderesult}} } }, {{ 'bag,edge={ed},edge label={node[arr]{ ' }} {{ '\mathtiny{%s}' % edgelabel if edgelabel }} } }
 {%- if children %}{%- for child_node in children %}
 {{ render_node(child_node) | indent(2,true) }}
 {%- endfor %}{%- endif %}
 ]
""")

# {{ output_node(self, true) }}
# name=%d, % hash(self)
tree_tmpl = env.from_string(r"""[ {\mathfootnote{ {{ '%s' % nodelabel if nodelabel }} }{ {{'%s' % proc if proc}}{{',\lambda: %s' % recale if recale}}{{',LUP(%s)' % Lookup if Lookup}}{{'\mapsto %s' % noderesult if noderesult}} } }, {{ 'bag,draw' }}
 {%- if children %}{%- for child_node in children %}
 {{ render_node(child_node) | indent(2,true) }}
 {%- endfor %}{%- endif %}
]""")

# {{ output_node(self, true) }}
mypic_tree_tmpl = env.from_string(r"""\node[bag,draw]  {{ '(%d)' % hash(self) }}
{
  {{ '$%s$' % nodelabel if nodelabel }}  \mathfootnote{  {{ ' %s: ' % proc if proc }}  {{ ' %s ' % nodelabel if nodelabel }}  {{ ' \mapsto %s ' % noderesult if noderesult }}  {{ ' (rescale: %s) ' % rescale if rescale }}  {{ ' (look\_up\_poly: %s) ' % look_up_poly if look_up_poly }}  }
}
{%- if children %}{%- for child_node in children %}
child
{
{{ render_node(child_node) | indent(2, true) }}
}
{%- endfor %}{%- endif %}
;
""")
##  {% if loop.last %} ; {% endif %}

def render_node(data):
    #print data['nodename']
    return node_tmpl.render(**data)

env.globals.update(render_node=render_node, hash=hash) # , output_node=output_node)

tikz_begin = r"""% \noindent\begin{mypic}
\noindent\begin{minipage}{\textwidth}\noindent\Forest{
for tree={grow=0,anchor=west,child anchor=west,l=2.3cm}"""

tikz_end = r"""}\end{minipage}
% \end{mypic}
"""

def generate(data):
    print tikz_begin
    print tree_tmpl.render(**data)
    print tikz_end



header = r"""
\documentclass[a3paper,6pt,landscape]{extbook}

\usepackage[latin1]{inputenc}
\usepackage{tikz}
\usepackage{forest}
\usetikzlibrary{trees,automata,positioning,arrows,shapes,shadows,shapes.multipart}
\usepackage{verbatim}

\usepackage{amsmath}
\usepackage{xspace}
\usepackage{geometry}
\geometry{
  margin=2.5cm
}
%   includeheadfoot,

\usepackage{color}   %May be necessary if you want to color links
\usepackage{hyperref}
\hypersetup{
  colorlinks=true, %set true if you want colored links
  linktoc=all,     %set to all if you want both sections and subsections linked
  linkcolor=blue,  %choose some color if you want links to stand out
}


\usepackage[para,perpage]{footmisc}
\usepackage{pifont}
\renewcommand*{\thempfootnote}{\ding{\numexpr171+\value{mpfootnote}}}
% \renewcommand*{\thempfootnote}{\underline{\roman{mpfootnote}}}



\newcommand{\MYleft}[1]{\left #1}
\newcommand{\MYright}[1]{\right #1}
\newcommand{\LBRACKET}{\MYleft(}
\newcommand{\RBRACKET}{\MYright)}

\newcommand{\LANGLE}{\MYleft\langle}
\newcommand{\RANGLE}{\MYright\rangle}

\newcommand{\INBRACKETS}[1]{\ensuremath{\LBRACKET \mathclose{#1}\RBRACKET}}
\newcommand{\FUNCTION}[2]{\ensuremath{#1\mathclose{\INBRACKETS{#2}}}\xspace}

\newcommand{\GEN}[1]{\ \ensuremath{\mathbf{\pmb{\varepsilon}_{#1}}}}
\newcommand{\GENP}[1]{\ \ensuremath{\mathbf{\pmb{\epsilon}_{#1}}}}

%\DeclareMathOperator{\OPGEN}{gen}  % module generator in 1st syzygy module
%\newcommand{\GEN}[1]{\FUNCTION{\OPGEN}{#1}}
%\newcommand{\GENP}[1]{\FUNCTION{\OPGEN'}{#1}}
%\DeclareMathOperator{\OPGEN}{e}  % module generator in 1st syzygy module
%\DeclareMathOperator{\OPGENP}{e'}  % module generator in 1st syzygy module
%\newcommand{\GEN}[1]{\ensuremath{\mathbold{\OPGEN_{#1}}}}
%\newcommand{\GENP}[1]{\ensuremath{\mathbold{\OPGENP_{#1}}}}


\newcommand{\INANGLES}[1]{\ensuremath{\LANGLE\mathclose{#1}\RANGLE}}
\newcommand{\GENERATED}[1]{\INANGLES{#1}}

\setcounter{tocdepth}{4}

\newenvironment{mypic}
{
  \begin{minipage}{\textwidth}\noindent\begin{tikzpicture}[grow=right,shorten >=2pt,on grid,auto]
}
{
  \end{tikzpicture}\end{minipage}
}

% Set the overall layout of the tree
%\tikzstyle{level 1}=[level distance=3.5cm, sibling distance=3.5cm]
%\tikzstyle{level 2}=[level distance=3.5cm, sibling distance=2cm]

% Define styles for bags and leafs
%\tikzstyle{bag} = [text width=4em, text centered]
%\tikzstyle{end} = [circle, minimum width=3pt,fill, inner sep=0pt]

% Set the overall layout of the tree
%\tikzstyle{level 1}=[level distance=3cm, sibling distance=2.5cm]
%\tikzstyle{level 2}=[level distance=2.8cm, sibling distance=2.5cm]
%\tikzstyle{level 3}=[level distance=2.5cm, sibling distance=2cm]
%\tikzstyle{level 4}=[level distance=2.5cm, sibling distance=1.5cm]
%\tikzstyle{level 5}=[level distance=2.5cm, sibling distance=1.5cm]
%\tikzstyle{level 6}=[level distance=2cm, sibling distance=1cm]

% Define styles for bags and leafs
%\tikzstyle{bag}=[text centered,black]
%\tikzstyle{arr}=[draw=none,above=1pt,sloped,anchor=base,pos=0.65,opacity=1,gray]
%%
%\tikzstyle{end} = [circle, minimum width=4pt,fill, inner sep=1pt]
%\tikzstyle{ed} =[draw,->,very thin,opacity=1,black]

\tikzstyle{bag}=[inner sep=1pt,black,rounded rectangle,text centered]
% ,fit=tight?, text centered, , minimum height=2cm,minimum width=3cm  , split

%\newcommand{\mathfootnote}[2]{
% {\ensuremath{
%   \stackrel{
%    \phantom{a}\footnote{\begingroup\everymath{\scriptstyle}\scriptsize\ensuremath{{#2}}\endgroup}
%   }{#1}
%  }
% }
%}

%  \nodepart{lower}   \phantom{1}
% overset

\newcommand{\mathfootnote}[2]{
\ensuremath{#1}\footnote{\begingroup\everymath{\scriptstyle}\scriptsize\ensuremath{#2}\endgroup}
}



%\newcommand{\mathtiny}[1]{\begingroup\everymath{\scriptstyle}\scriptsize\ensuremath{{#1}}\endgroup}
%\newcommand{\mathfootnote}[1]{\ \footnote{\begingroup\everymath{\scriptstyle}\scriptsize\ensuremath{{#1}}\endgroup} }


\newcommand{\mathtiny}[1]{\begingroup\everymath{\scriptstyle}\scriptsize\ensuremath{{#1}}\endgroup}
% \newcommand{\mathtiny}[1]{\ensuremath{{#1}}}

\tikzstyle{arr}=[draw=none,above=2pt,sloped,anchor=base,pos=0.59,opacity=1,gray,font=\scriptsize]
%%
%\tikzstyle{end} = [circle, minimum width=4pt,fill, inner sep=1pt]
\tikzstyle{ed} =[dashed,->,thin,opacity=0.5,black]


%
%\newenvironment{myforest}
%{ \noindent\begin{minipage}{\textwidth}\noindent\begin{forest} for tree={grow=0,anchor=west,child anchor=west,l=2.3cm} }
%{ \end{forest}\end{minipage}}
%\newcommand{myforest}[1]{
%\noindent\begin{minipage}{\textwidth}\noindent\Forest{for tree={grow=0,anchor=west,child anchor=west,l=2.3cm} #1}\end{minipage}
%}
%\myforest{

\raggedbottom
\interfootnotelinepenalty=10000


\begin{document}
\frontmatter
% \pagestyle{empty}
% \chapter*{Content}\addcontentsline{toc}{chapter}{Content}
\tableofcontents

\mainmatter
"""

footer = r"""
\backmatter
% \appendix
\end{document}
"""


sections = { -1: '%% -----  ',  0: '\\newpage\\chapter', 1: '\\section', 2: '\\subsection', 3: '\\subsubsection'}

def printHeader(k, i):
    if i not in sections:
	i = -1

    print( sections[i] + "{" + k + "}\n" );


def generateSyzList(data):

    for s in data:
        assert( "syzygylayer" in s );
        assert( "diagrams" in s );

        printHeader("$SYZ^{" + str( s["syzygylayer"] +"}(M):$" ), 2)

        for (i, d) in enumerate(s["diagrams"], start = 1):
            assert( "nodelabel" in d );

            if( "noderesult" in d ):
                printHeader("({" + str(i) + "}): $" + str(d["nodelabel"]) + " \mapsto "+ str(d["noderesult"]) + "$", 3)
            else:
                printHeader("({" + str(i) + "}): $" + str(d["nodelabel"]) + " \mapsto \ldots $", 3)

            generate(d)


def generateRes(data):
    for (k, v) in data.iteritems():
        # no special name here: "desc" : [{}] == SyzList
        printHeader(k, 1)
        generateSyzList(v)

def generateExample(e):
    assert ("Example" in e);
    assert ("computations" in e);

    printHeader(e["Example"], 0)

    for c in e["computations"]:
        generateRes(c)

def generateTests(data):
    # Top-most level: "EXAMPLES": [ {} ]

    for (k, v) in data.iteritems():
        printHeader(k, -1)
        for e in v:
            generateExample(e)



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_file", help="name of input file containing json formatted tree")
    parser.add_argument("-d", "--dump", help="dump json file", action="store_true")
    parser.add_argument("-v", "--view", help="view raw string", action="store_true")
    parser.add_argument("-g", "--gen", help="generate latex", action="store_true")
    parser.add_argument("-f", "--full", help="output full latex file", action="store_true")
    parser.add_argument("-r", "--res", help="input file contains resolutions", action="store_true")
    parser.add_argument("-t", "--tests", help="input file contains multiple tests / examples / resolutions", action="store_true")

    args = parser.parse_args()

    raw = open(args.input_file, "r").read().replace('\n', '')

    raw = re.sub(r', *}', r'}',    raw)
    raw = re.sub(r', *]', r']',    raw)
    raw = re.sub(r'\@', r'\\\@',     raw)
##     raw = re.sub(r'_', r'\\_',     raw)
    raw = re.sub(r'\%', r'\\\%',     raw)
    raw = re.sub(r'\\*cdot', r'',     raw)
###    raw = re.sub(r'\\',   r'\\\\', raw)

    if args.view:
        print (raw)

    data = simplejson.loads(raw)

    if args.dump:
        print( simplejson.dumps(data, indent=3 * ' ') )

    elif args.gen:
        if args.full:
            print header

        if args.tests:
            generateTests(data)
        elif args.res:
            generateRes(data)
        else:
            generate(data)

        if args.full:
            print footer



