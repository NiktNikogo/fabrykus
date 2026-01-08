#include "simpleNode.hpp"

template<unsigned int N>
SimpleNode<N>::SimpleNode() : BaseNode() {
    setTitle("Simple sum");
    setStyle(ImFlow::NodeStyle::green());
    ImFlow::BaseNode::addIN<int>("In", 0, ImFlow::ConnectionFilter::SameType());
    ImFlow::BaseNode::addOUT<int>("Out", nullptr)
        ->behaviour([this]() { return getInVal<int>("In") + m_valB; });
}


template <unsigned int N>
void SimpleNode<N>::draw()
{
    ImGui::Text(std::format("UID: 0x{:X}", this->getUID()).c_str());
    ImGui::SetNextItemWidth(100.f);
    ImGui::InputInt("##ValB", &m_valB);
    ImGui::SetNextItemWidth(100.f);
    ImGui::InputInt("##ValA", &m_valA);
    for(auto i = 0; i < N; i++) {
        ins[i] = ImFlow::BaseNode::showIN<int>(std::format("In {}", i), 0, ImFlow::ConnectionFilter::SameType());
    }
    
}

template class SimpleNode<2>;
